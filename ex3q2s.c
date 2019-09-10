
/*
NAME: Shachar Israeli
ID: 203713094
DATE: 19/05/2018
*/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <math.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_SIZE 1000

int parallelism = 0;
int array_size = 0;
int blocks_to_sort = 0;


int* input_file(int argc, char* argv[]);
void print_before_sort(int *array_before_sort);
int* divide_array(int *array, int start);
void make_sockets(int *array, int argc, char* argv);
void copy_after_merge(int *array, int* aftermerge,int start);

void mergeSort(int* arr,int size, int l, int r);
void merge(int* arr, int l, int m, int r);
void print_after(int* sorted_array);



void error(char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[])
{

    int* array_before_sort = input_file(argc, argv);
    print_before_sort(array_before_sort);
    make_sockets(array_before_sort, argc, argv[2]);
    free (array_before_sort);

    return 0;
}



int* input_file(int argc, char* argv[])
{
    FILE *file_input;

    if (argc < 2)
    {
        perror("invalid input file");
    }

    file_input = fopen(argv[1], "r");
    if (file_input == NULL)
    {
        perror("Can't open file");
    }


    fscanf(file_input, "%d\n%d", &array_size, &parallelism);
    int *array = (int*)malloc((array_size) * sizeof(int));
    assert(array != NULL);
    for (int i = 0; i < array_size ; i++)
    {
        fscanf(file_input, "%d,", &array[i]);
    }

    blocks_to_sort = (array_size / parallelism);       // how many numbers need to send in every array that we send
    
    //if (parallelism > array_size)
	//	blocks_to_sort;
    
    
    fclose(file_input);
    return array;
}



void print_before_sort(int *array_before_sort)
{
    printf("Amount of numbers that sort: %d\n", array_size);
    printf("Degree of parallelism: %d\n", parallelism);
    int i;
    printf("Before sort: ");
    for (i = 0; i < array_size-1; i++)
    {
        printf("%d,", array_before_sort[i]);
    }
    printf("%d\n", array_before_sort[i]);
}


void make_sockets(int *array, int argc, char* argv)
{
	
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr;
    struct sockaddr cli_addr;
    socklen_t clilen;
    int n,rc;

    if (argc < 3)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv));


    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    fd_set rfd; 		/* set of open sockets */
    FD_ZERO(&rfd);

   
    if(listen(sockfd,parallelism) == -1)
	{
		perror("listen() failed");
		exit(0);
	}
  
	int p_start = 0;  // will save the begin pointer of the array that we send
	for(int i = 0; i < parallelism; i++)
	{
		struct sockaddr_in cli_addr;
			//  newsockfd = accept(sockfd, &cli_addr, &clilen);  //open socket to new client
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if (newsockfd < 0)
       		perror("ERROR on accept");
                
            
		char ip[IP_SIZE];
		inet_ntop(AF_INET,&cli_addr.sin_addr,ip,sizeof(ip));
		printf("Got request from %s\n",ip);
           
		printf("Sending '%d' numbers to socket '%d'\n", blocks_to_sort, newsockfd);
					  
		int *arrayToSend = divide_array(array, p_start); 
         	           		
        write(newsockfd, &blocks_to_sort, sizeof(int));   // send the size of the array
            
		write(newsockfd, arrayToSend, blocks_to_sort*sizeof(int));   // send the array
		    

            // add the new socket to the set of open sockets
       FD_SET(newsockfd, &rfd);
             
       
		p_start = p_start + blocks_to_sort;  // pointer ++ 
		free (arrayToSend);
	}
		
	p_start = 0; // pointer restore
		
        // check which sockets are ready for reading
	for (int j = sockfd+1; j < sockfd + parallelism+1; j++)
	{
        	
		if (FD_ISSET(j, &rfd))
		{
			printf("Read from socket '%d'\n",j);
			int* arrayFromClient = (int*)malloc(blocks_to_sort * sizeof(int));
			assert(arrayFromClient != NULL);
			rc = read(j,arrayFromClient, sizeof(int)*blocks_to_sort);
                                                        // read the message
			copy_after_merge(array, arrayFromClient,p_start);
			p_start = p_start + blocks_to_sort;
			free (arrayFromClient);
             
        	if (rc == 0)
				close(j);
			FD_CLR(j, &rfd);
		}
		close(j);

	}
    

	close(sockfd);
	if(parallelism != 1 )
		mergeSort( array,array_size, 0, array_size-1);
    print_after(array);    
    
}


int* divide_array(int *array, int start)  //copy the part of the array that we want to send
{
    
    int* arr_c = (int*)malloc(blocks_to_sort * sizeof(int));
	assert(arr_c!= NULL);
	
    for(int i = start, j = 0 ; i < start+blocks_to_sort && j< blocks_to_sort; i++, j++)
    {
        arr_c[j] = array[i];
    }
 
    return arr_c;
}






void copy_after_merge(int *array, int* aftermerge,int start) // copy the merge part to the origin array

{
	for(int i = start, j = 0 ; i < start+blocks_to_sort; i++, j++)
	{	
		array[i] = aftermerge[j];
        if(i == start+blocks_to_sort-1)
        	printf("%d\n",array[i]);
        else
        printf("%d,",array[i]);
     }   

}


void merge(int* arr, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
 
    /* create temp arrays */
    int L[n1], R[n2];
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 
/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int* arr,int size, int l, int r)
{
	int m = l+(r-l)/2;
	
    if (size/2 == blocks_to_sort)  // the place that we know the client already merged the array
    {
    	
    	merge(arr, l, m, r);
    	return;
    }
    
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
 
        // Sort first and second halves
        mergeSort(arr,size/2, l, m);
        mergeSort(arr,size/2, m+1, r);
 
        merge(arr, l, m, r);
    
}



void print_after(int* sorted_array)
{
	printf("Final answer: ");
    int i;
    for(i = 0; i < array_size - 1; i++)
        printf("%d,", sorted_array[i]);
	printf("%d\n", sorted_array[i]);       // print without , in the end
}






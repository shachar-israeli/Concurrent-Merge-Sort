

/*
NAME: Shachar Israeli
ID: 203713094
DATE: 19/04/2018
*/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>




void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);

void printArray(int* arrayToSend,int size, int checkEnd);

void error(char *msg)
{
    perror(msg);
    exit(0);
}


int main(int argc, char *argv[])
{
    int sockfd, n,num,rc;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)  &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sockfd,(const struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

   
	int blocks_to_sort;
	rc = read(sockfd, &blocks_to_sort,  sizeof(int)); // first get the size of the arr.
        
        
	int *arrayToSend = (int*)malloc(blocks_to_sort * sizeof(int));   //create the arr
	assert(arrayToSend!= NULL);
       	
	rc = read(sockfd, arrayToSend, blocks_to_sort *sizeof(int)); // get the array from server
       		      		
	printArray(arrayToSend, blocks_to_sort, 0);   // 0 - before              
	mergeSort(arrayToSend, 0, blocks_to_sort-1);  // merge the arr
	printArray(arrayToSend, blocks_to_sort, 1);  // 1 - after

	rc = write(sockfd, arrayToSend,  sizeof(int)*blocks_to_sort);  // return the arr after the merge
	close(sockfd);
	free(arrayToSend);  

	return 0;
}


// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
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
void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        merge(arr, l, m, r);
    }
}

void printArray(int* arrayToSend,int size, int checkEnd)
{
   int i;
   if ( checkEnd == 0)            // 0 for received
        printf("Array received: ");
    else
       printf("Array send: ");

    for (i = 0; i < size-1; i++)
    {
        printf("%d,", arrayToSend[i]);
    }
   printf("%d\n", arrayToSend[i]);   // print without the , in the end
}


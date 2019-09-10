
/*
NAME: Shachar Israeli
ID: 203713094
DATE: 19/05/2018


 get a array and sort it toghther with a help of childs.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>


void error(char*);
int* input_file(int, char**);
void print_before(int*);
int* shared_memory(int*, char*);
void mergeSort(int* arr, int l, int r);
void merge(int* arr, int l, int m, int r);
void print_after(int*);
void merge_with_childs(int * array);

void mergeOneSide (int * array, int size, int p_start, int p_until,int blocksToSort);

void mergeLeftAndRight ( int * arr , int start_L,int end_L , int start_R, int end_R);
int * copyArray ( int * arr);

int size = 0;
int paral = 0;
int shm_id;

int main(int argc, char *argv[])
{


    int* array_before_merge = input_file(argc, argv); // get the array from file
    print_before(array_before_merge);
    int* array_to_sort = shared_memory(array_before_merge, argv[1]);
    
	merge_with_childs(array_to_sort);       // start the recorsia 
	 
	print_after(array_to_sort);
	
	
	free( array_before_merge);         // free the pointer and the memory place
    shmdt(array_to_sort);
    
    if (shmctl(shm_id,IPC_RMID, NULL) == -1)
	{
		perror( "shmctl failed" ) ;
		
	}  

    return 0;
}


int* input_file(int argc, char* argv[])
{
    FILE* input;

    if (argc < 2)              // check if there is a file with the details to read
        error("Bad Number of parameters");

    input = fopen(argv[1], "r");  
    if (input == NULL)
        error("Can't open input file");

    int* arr;

    fscanf(input, "%d\n%d", &size, &paral);   /// get the size and paral from the first rows of the file

    arr = (int*)malloc(size * sizeof(int));       // will be the array to deal with
    if(arr == NULL)
        error("Error: could not allocate memory");


    int i;
    for(i = 0; i < size; i++)         // get all the numbers from the array 
    {
        fscanf(input, "%d,", &arr[i]);
    }

    fclose(input);     // close the file P

    return arr;
}


void merge_with_childs(int * array_to_sort)

{

	int blocksToSort = size / paral ;
	 
	if( size < paral)            // dont use forks and just merge it
	{
		mergeSort(array_to_sort, 0 , size-1 );	
	
	}
	 	 
	else if( paral == 1)     // just 1 fork to use
	{
	
		int status = 0;
		pid_t child_left;
		
		child_left = fork();
	
	
		if(child_left < 0 )
			error("Error in creating left child");

		else if(child_left == 0)
		{
     	   
			printf("Create a process: %d\n", getpid());
			mergeSort(array_to_sort, 0 , size-1 );		
			
			exit(0);
		}		
		waitpid(child_left, &status, 0);
		
	}
	 

	else if ( paral == 2)  // go inside only if the paral is 2
	{
	 	int status = 0;
		pid_t child_left,child_right;
		
	
		child_left = fork();
	
	
		if(child_left < 0 )
			error("Error in creating left child");         // take the array to 2 parts and after it merge it 

		else if(child_left == 0)
		{
     	   
			printf("Create a process: %d\n", getpid());
			mergeSort(array_to_sort, 0 , (size-1)/2 );		
			
			exit(0);
		}
		
		waitpid(child_left, &status, 0);
		child_right = fork();
		if(child_right < 0 )
			error("Error in creating left child");
			
		else if(child_right == 0)
     	   {
			printf("Create a process: %d\n", getpid());
			mergeSort(array_to_sort, size/2 , size-1);
			exit(0);
     	   }  	  

		waitpid(child_right, &status, 0);
			
		mergeLeftAndRight ( array_to_sort, 0, size/2 , size / 2, size);
			
	}
	
	else      // go inside untill find the right place to fork
	{
		mergeOneSide (array_to_sort,size/2,0,(size-1)/2,blocksToSort);      // take the first half of the array    
		mergeOneSide (array_to_sort,size/2,size/2,size-1,blocksToSort);    // take the secound half of the array  		
		mergeLeftAndRight ( array_to_sort, 0, size/2 , size / 2, size);    // merge them both after the sort	 
	}

}

void print_before(int* array)
{
    printf("Amount of numbers that sort: %d\n", size);     // print the details before the merge
    printf("Degree of parallelism: %d\n", paral);
    int i;
    printf("Before sort: ");
    for (i = 0; i < size - 1; i++)
        printf("%d,", array[i]);

    printf("%d\n", array[i]);
}


int* shared_memory(int* array_to_sort,char* argv)      // create the shared memory and save the array there
{
    key_t key;
    if ((key = ftok(argv, 'y')) == -1)
        error("ftok() failed");

    
    shm_id = shmget(key, sizeof(int) * size, IPC_CREAT  | 0600);
    if (shm_id == -1)
        error("shmget()first failed");

    int *shm_ptr;
    shm_ptr = (int *) shmat(shm_id, NULL, 0);
    if (*shm_ptr == -1)
        error("shmat failed");

    for (int i = 0; i < size; i++)
    {
        shm_ptr[i] = array_to_sort[i];
    }

    return shm_ptr;               // return the sh memory with a pointer
}


void print_after(int* sorted_array)       // print after the merge
{
    printf("After sort: ");
    int i;
    for(i = 0; i < size - 1; i++)
        printf("%d,", sorted_array[i]);

    printf("%d\n", sorted_array[i]);       // print without , in the end
}

void error(char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}
   
    
void mergeOneSide (int * array, int size, int p_start, int p_until,int blocksToSort)
{
	 int m = p_start+(p_until-p_start)/2;  // give me the middle
	 
	if ( size / 2 == blocksToSort)  // we in the right place to do the fork
	{
	 	int status=0;
		pid_t child_left,child_right;
		
	
		child_left = fork();
	
	
		if(child_left < 0 )
			error("Error in creating left child");

		else if(child_left == 0)  // give a half to this process
		{
     	   
			printf("Create a process: %d\n", getpid());
			mergeSort(array, p_start , m );
						
			exit(0);
		}
		
		waitpid(child_left, &status, 0);    // wait untill the merge is finished
		child_right = fork();
		if(child_right < 0 )
			error("Error in creating left child");
			
		else if(child_right == 0)
     	   {
			printf("Create a process: %d\n", getpid());
			mergeSort(array, m+1 , p_until);              // give the other half to this process
			exit(0);
     	   }  	  

			waitpid(child_right, &status, 0);
			
			mergeLeftAndRight ( array, p_start, m+1 , m+1,  p_until+1 );  // merge them both after 2 forks done			
	}
	
	else
	{	
		mergeOneSide ( array,  size/2,  p_start, m,blocksToSort); //take another half of the arr and find if the process place is there
		mergeOneSide ( array, size/2, m+1,  p_until,blocksToSort); 
		mergeLeftAndRight ( array, p_start, m+1 , m+1 , p_until+1); // after every "cut" we need to merge the pices toghter
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
void mergeSort(int* arr, int l, int r)
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



int * copyArray ( int * arr)  // just copy the array to temp array. use in the mergeleftandright
{
	int temp[size];
	
	for(int i = 0; i < size; i++)
        temp[i] = arr[i];
	
	int *p = temp;
	return p;
}


void mergeLeftAndRight ( int * arr , int start_L,int end_L , int start_R ,int end_R)

{        // will merge 2 sorted arrays to 1 sort array

	int* temp = copyArray (arr);   /// make temp array


	int i = start_L; // Initial index of first subarray
	int j = start_R ; // Initial index of second subarray
	int k = start_L; // Initial index of merged subarray
	
    while (i < end_L && j < end_R)
    {
        if (temp[i] <= temp[j])
        {
            arr[k] = temp[i];
            i++;
        }
        else
        {
            arr[k] = temp[j];
            j++;
        }
        k++;
    }
 

 
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < end_L)
    {
         arr[k] = temp[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < end_R)
    {
        arr[k] = temp[j];
        j++;
        k++;
    }
   
}









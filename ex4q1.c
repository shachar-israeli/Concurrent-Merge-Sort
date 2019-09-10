

/*
NAME: Shachar Israeli
ID: 203713094
DATE: 19/05/2018
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
#include <pthread.h>

#define P_SIZE 2 


void error(char*);
int* input_file(int, char**);
void print_before(int*);
void mergeSort(int* arr, int l, int r);
void merge(int* arr, int l, int m, int r);
void print_after();
void merge_with_threads();

void mergeOneSide (int size, int p_start, int p_until,int blocksToSort);

void mergeLeftAndRight (int start_L,int end_L , int start_R, int end_R);
int * copyArray ( int * arr);

void* threadCallMerge (void* L_R_pointers);

int size = 0;  // save the size of the array
int paral = 0;   // get how many threads we need to do

int* array_to_sort;  // the global array
int main(int argc, char *argv[])
{


    array_to_sort = input_file(argc, argv);    // now i have the array ready to go
    print_before(array_to_sort);  
   
    
	merge_with_threads();       // start the recorsia 
	 
	print_after(array_to_sort);
	
	free( array_to_sort); 
      

    return 0;
}

int* input_file(int argc, char* argv[])
{
    FILE* input;

    if (argc < 2)
        error("Bad Number of parameters");

    input = fopen(argv[1], "r");      // get the file into input
    if (input == NULL)
        error("Can't open input file");

    int* arr;

    fscanf(input, "%d\n%d", &size, &paral);    // get the parmeters to size and paral

    arr = (int*)malloc(size * sizeof(int));
    if(arr == NULL)
        error("Error: could not allocate memory");


    int i;
    for(i = 0; i < size; i++)
    {
        fscanf(input, "%d,", &arr[i]);         // get the numbers to the array
    }

    fclose(input);

    return arr;
}


void merge_with_threads()

{

	 int blocksToSort = size / paral ;       // what is the size of the array that we need to sort every thread
	 
	if ( size < paral)   // just merge
	{
	
		mergeSort( array_to_sort, 0, size-1);
	
	}
	 
	else if (paral == 1 )    // 1 thread
	{
		pthread_t thread1;
		int status = 1 ;    // save the start pointer and the end pointer into array.
		int pointers[P_SIZE];
		pointers[0] = 0;
		pointers[1] = size-1;
		
		
		status = pthread_create(&thread1, NULL, threadCallMerge,(void*) pointers );
		if (status != 0)
		{
		fputs("pthread create failed in main", stderr);
		exit(EXIT_FAILURE);
		}	
			pthread_join(thread1,NULL);	
		
	} 
	 

	else if ( paral == 2)  // go inside only if the paral is 2
	{
	 	pthread_t thread1, thread2;
		int status =1 ;
		int pointersL[P_SIZE];
		int pointersR[P_SIZE];
		
		pointersL[0] = 0;
		pointersL[1] = (size-1)/2;      //    // save the start pointer and the end pointer into array.
		pointersR[0] = size/2;
		pointersR[1] = size-1;
		
		
		status = pthread_create(&thread1, NULL, threadCallMerge,(void*) pointersL ); // send the part of the array into new method 
		if (status != 0)
		{
		fputs("pthread create failed in main", stderr);
		exit(EXIT_FAILURE);
		}	
		
		
		status = pthread_create(&thread2, NULL, threadCallMerge,(void*) pointersR );
		if (status != 0)
		{
		fputs("pthread create failed in main", stderr);
		exit(EXIT_FAILURE);
		}	
		
		pthread_join(thread1,NULL);  // wait to both of the methods and after this merge them toghther
		pthread_join(thread2,NULL);
		
			
		mergeLeftAndRight (0, size/2 , size / 2, size);
			
	}
	
	else      // go inside untill find the right place to fork
	
	{


    mergeOneSide (size/2,0,(size-1)/2,blocksToSort);      // divide the array to 2 
    
   	mergeOneSide (size/2,size/2,size-1,blocksToSort);
   		
	mergeLeftAndRight ( 0, size/2 , size / 2, size);  // merge them
	
	}


}

void print_before(int* array)
{
    printf("Amount of numbers that sort: %d\n", size);
    printf("Degree of parallelism: %d\n", paral);
    int i;
    printf("Before sort: ");
    for (i = 0; i < size - 1; i++)
        printf("%d,", array[i]);

    printf("%d\n", array[i]);
}



void print_after()
{
    printf("After sort: ");
    int i;
    for(i = 0; i < size - 1; i++)
        printf("%d,", array_to_sort[i]);

    printf("%d\n", array_to_sort[i]);       // print without , in the end
}

void error(char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

    
    
void mergeOneSide (int size, int p_start, int p_until,int blocksToSort)
{
	 int m = p_start+(p_until-p_start)/2;  // give me the middle
	 
	if ( size / 2 == blocksToSort)  // we in the write place to do the fork
	{
	 	
		pthread_t thread1, thread2;
		int status = 1 ;
		int pointersL[P_SIZE];                // will save the pointers to the merge sort : [0] - begin [1] - end
		int pointersR[P_SIZE];
		
		pointersL[0] = p_start;      // save the start pointer and the end pointer into array.
		pointersL[1] = m;
		pointersR[0] = m+1;
		pointersR[1] = p_until;
		
		
		status = pthread_create(&thread1, NULL, threadCallMerge,(void*) pointersL );
		if (status != 0)
		{
		fputs("pthread create failed in main", stderr);
		exit(EXIT_FAILURE);
		}	
		
		
		status = pthread_create(&thread2, NULL, threadCallMerge,(void*) pointersR );
		if (status != 0)
		{
		fputs("pthread create failed in main", stderr);
		exit(EXIT_FAILURE);
		}	
		
		pthread_join(thread1,NULL);  // wait to the threads to finish
		pthread_join(thread2,NULL); 
		
		
		mergeLeftAndRight (p_start, m+1 , m+1,  p_until+1 );  // merge them both
			
	}
	
	else
	
	{
	

	mergeOneSide (  size/2,  p_start, m,blocksToSort);   // take another half of the arr and find if the process place is there
	mergeOneSide (  size/2, m+1,  p_until,blocksToSort);
	mergeLeftAndRight ( p_start, m+1 , m+1 , p_until+1);
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



int * copyArray ( int * arr)  // just copy the array to temp array
{
	int temp[size];
	
	for(int i = 0; i < size; i++)
        temp[i] = arr[i];
	
	int *p = temp;
	return p;
}


void mergeLeftAndRight (int start_L,int end_L , int start_R ,int end_R)

{

	int* temp = copyArray (array_to_sort); 


	int i = start_L; // Initial index of first subarray
	int j = start_R ; // Initial index of second subarray
	int k = start_L; // Initial index of merged subarray
    while (i < end_L && j < end_R)
    {
        if (temp[i] <= temp[j])
        {
            array_to_sort[k] = temp[i];
            i++;
        }
        else
        {
            array_to_sort[k] = temp[j];
            j++;
        }
        k++;
    }
 

 
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < end_L)
    {
         array_to_sort[k] = temp[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < end_R)
    {
        array_to_sort[k] = temp[j];
        j++;
        k++;
    }
   

}

void* threadCallMerge (void* L_R_pointers)
{
	printf("Create a thread: %u\n", (unsigned int)pthread_self());   
	int* p = (int*) L_R_pointers;      // take the begin and end from the array to int
	int l = p[0];
	int r = p[1];
	
	mergeSort(array_to_sort, l, r);   // start the merge 
	
	pthread_exit(NULL);     // return nothing to the array
}










israelish
203713094
Shachar Israeli

########################################################


This software is divided into 2 assignments:
assignment 1: a Concurrent merge sort using shared memory
assignment 2: a Concurrent merge sort using sockets





#### how to install assignment 1 ####
open linux terminal, navigate to the folder containing ex3q1.c
using the "cd" command (confirm it by using ls command)
incase you have makefile, type make and the program will
automaticily be compiled, if you don't, type gcc -Wall ex3q1.c -o ex3q1
and your program will automaticily be compiled

#### how to install assignment 2 ####

open linux terminal, navigate to the folder containing ex3q2s.c and ex3q2c.c
using the "cd" command (confirm it by using ls command)
incase you have makefile, type make and the program will
automaticily be compiled, if you don't, type gcc -Wall ex3q2s.c -o ex3q2s
and your program will automaticily be compiled.
after you compile the server, open another linux terminal
and type make and the program will automaticily be compiled,
if you don't, type gcc -Wall ex3q2c.c -o ex3q2c
and your progam will automaticily be compiled. 



#### to activate assignment 1 #### 
open linux terminal, navigate to ex3 executeable file
location using "cd" command (confirm it using ls command) and type
./ex3q1 *name of input file* (remove *)


#### to activate assignment 1 #### 
open linux terminal, navigate to ex3 executeable file
location using "cd" command (confirm it using ls command) and type
./ex3q2s *name of input file* (remove *)

open another linux terminal, navigate to ex3 executeable file
location using "cd" command (confirm it using ls command) and type
./ex3q2c localhost *port number* (remove *)





##################  assignment 1 functions ##################


* incase of memory allocation failure, program will exit with
message "Memory allocation failed"

void error(char*);
input: char[] - error messege
output: prints in stderr format the error


int* input_file(int, char**);
input: int - argument counter, char** - list in arguments
output: returns the array from the input file

void print_before(int*);
input: int* - the array of numbers before sort
output: prints to the terminal the array

int* shared_memory(int*, char*);
input: int* - the array we need to insert into the shared memory section, char* - the name of file key
output: returns the array in the shared memory section


void mergeSort(int* arr, int l, int r);
input: int* - the array to be sorted, int - left index, int - right index
output: sorting the array


void merge_sort_fork(int*);
input: int* - the array to be sorted
output: sorting the array


void merge(int*, int , int , int );
input: int* - the array to sort, int - left index, int - middle index, int - right index
output: merging the sub-arrays


void merge_with_childs(int * array);
input: int* - the array to sort, int - start left array index, int - end left array index, int - start right array index, int - end right array index
output: merging the sub-arrays


void mergeOneSide (int * array, int size, int p_start, int p_until,int blocksToSort);
input: int* - the array to sort, int - start left array index, int - end left array index, int - start right array index, int - end right array index
output: merging the sub-arrays


int* copy_array(int*);
input: int* - array to be copied
output: returns a copied array


void print_after(int*);
input: int* - the sorted array
output: prints the sorted array

int * copyArray ( int * arr) - copy the array to use it in the mergeLeftAndRight

int main(int argc, char* argv[]) command:
input: char* argv[] - the arguments, int argc - number of argument
output: concurrent merge sort that has been fully tested with valgrind to test for memory
leak, no leak was found.


------------------assignment 2 server functions -------------------------------
* incase of memory allocation failure, program will exit with
message "Memory allocation failed"



int* input_file(int argc, char* argv[]);
void print_before_sort(int *array_before_sort);
int* divide_array(int *array, int start);
void make_sockets(int *array, int argc, char* argv);
void copy_after_merge(int *array, int* aftermerge,int start);

void mergeSort(int* arr,int size, int l, int r);
void merge(int* arr, int l, int m, int r);
void print_after(int* sorted_array);


int main(int argc, char* argv[]) command:
input: char* argv[] - the arguments, int argc - number of argument
output: concurrent merge sort that has been fully tested with valgrind to test for memory
leak, no leak was found.


------------------assignment 2 client functions -------------------------------
* incase of memory allocation failure, program will exit with
message "Memory allocation failed"

void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);

void printArray(int* arrayToSend,int size, int checkEnd);


int main(int argc, char* argv[]) command:
input: char* argv[] - the arguments, int argc - number of argument
output: concurrent merge sort that has been fully tested with valgrind to test for memory
leak, no leak was found.



good luck and i hope all is good,
Shachar israeli :)

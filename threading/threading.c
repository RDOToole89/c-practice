#include <stdio.h>      // Standard I/O functions (printf, fprintf)
#include <stdlib.h>     // Memory allocation functions (malloc, free), exit, and EXIT_FAILURE
#include <pthread.h>    // POSIX threads functions (pthread_create, pthread_join)
#include <time.h>       // Time-related functions (time, srand) for seeding rand

// Define constants for the number of threads to use and the array size.
#define NUM_THREADS 4
#define ARRAY_SIZE 1000

//-------------------------------
// Structure: ThreadData
// Purpose: Holds the data each thread will work with.
// Members:
//   - array: A pointer to the shared array of integers.
//   - start: The starting index (inclusive) for this thread's work.
//   - end:   The ending index (exclusive) for this thread's work.
//-------------------------------
typedef struct {
  int* array;
  int start;
  int end;
} ThreadData;

//-------------------------------
// Function: partial_sum
// Purpose:  Thread function that computes the sum of a portion of the array.
// Input:    A void pointer that is actually cast to ThreadData*.
// Output:   Returns a dynamically allocated pointer to a long containing the partial sum.
// Difficult Concepts:
//   - Type casting void* to a proper structure pointer.
//   - Dynamically allocating memory for returning a result from a thread.
//   - Using pthread_exit to properly end the thread and return data.
//-------------------------------
void* partial_sum(void* arg) {
  // Cast the generic void pointer to our ThreadData pointer so we can access the thread-specific data.
  ThreadData* data = (ThreadData*)arg;
  long sum = 0;

  // Loop over the subset of the array determined by 'start' and 'end'
  for (int i = data->start; i < data->end; i++) {
    sum += data->array[i];
  }

  // Allocate memory to hold the result of type long.
  // This memory is allocated on the heap so it can be returned to the main thread.
  long* result = malloc(sizeof(long));
  if (result == NULL) {
    perror("Failed to allocate memory for result");
    // If allocation fails, exit the thread with an error.
    pthread_exit(NULL);
  }
  *result = sum;  // Store the calculated sum in the allocated memory.

  // Terminate this thread, returning the pointer to the sum.
  pthread_exit((void*)result);
}

//-------------------------------
// Function: main
// Purpose:  Sets up the shared data, creates threads to compute partial sums,
//           collects the results, and prints the total sum.
// Difficult Concepts:
//   - Dynamic memory allocation and clean-up (free).
//   - Dividing work among threads (calculating start and end indices).
//   - Error checking of pthread_create and pthread_join.
//   - Casting between void* and other pointer types.
//-------------------------------
int main() {
  // Seed the random number generator with the current time.
  srand(time(NULL));

  // Dynamically allocate an integer array of ARRAY_SIZE.
  int* array = malloc(ARRAY_SIZE * sizeof(int));
  if (array == NULL) {
    perror("Failed to allocate memory for array");
    exit(EXIT_FAILURE);
  }

  // Initialize the array with sequential integers (0 to ARRAY_SIZE - 1).
  for (int i = 0; i < ARRAY_SIZE; i++) {
    array[i] = i;
  }

  // Declare arrays to hold thread IDs and the thread-specific data.
  pthread_t threads[NUM_THREADS];
  ThreadData thread_data[NUM_THREADS];

  // Calculate how many array elements each thread should process.
  int chunk = ARRAY_SIZE / NUM_THREADS;

  // Create threads and assign each a portion of the array.
  for (int i = 0; i < NUM_THREADS; i++) {
    // All threads work on the same shared array.
    thread_data[i].array = array;
    // Set the starting index for this thread.
    thread_data[i].start = i * chunk;
    
    // For the last thread, ensure it processes any leftover elements.
    if (i == NUM_THREADS - 1) {
      thread_data[i].end = ARRAY_SIZE;
    } else {
      // Otherwise, set the ending index based on the current chunk.
      thread_data[i].end = (i + 1) * chunk;
    }

    // Create a new thread running the partial_sum function, 
    // passing the thread-specific data as an argument.
    int rc = pthread_create(&threads[i], NULL, partial_sum, (void*)&thread_data[i]);
    if (rc) {
      // If pthread_create fails, print an error message and clean-up.
      fprintf(stderr, "Error: pthread_create() failed with code %d\n", rc);
      free(array);
      exit(EXIT_FAILURE);
    }
  }

  // Variable to accumulate the total sum of the array.
  long total_sum = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    // Pointer to collect the result returned by each thread.
    void* result;
    // Wait for each thread to complete and obtain its return value.
    int rc = pthread_join(threads[i], &result);
    if (rc) {
      // If pthread_join fails, print an error, free memory and exit.
      fprintf(stderr, "Error: pthread_join() failed with code %d\n", rc);
      free(array);
      exit(EXIT_FAILURE);
    }

    // Dereference the returned pointer (cast to long*) and add the value to total_sum.
    total_sum += *(long*)result;

    // Free the memory allocated by the thread for its result.
    free(result);
  }

  // Print the total sum of the array.
  // Note: This includes a minor spelling mistake in the original print string ("arrau" -> "array").
  printf("The total sum of the array is %ld\n", total_sum);

  // Free the memory allocated for the array.
  free(array);

  return 0;
}

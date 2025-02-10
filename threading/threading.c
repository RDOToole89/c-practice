#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4
#define ARRAY_SIZE 1000

typedef struct {
  int* array;
  int start;
  int end;
} ThreadData;

void* partial_sum(void* arg) {

  ThreadData* data = (ThreadData*)arg;
  long sum = 0;

  for (int i = data->start; i < data->end; i++) {
    sum += data->array[i];
  }

  long* result = malloc(sizeof(long));
  if (result == NULL) {
    perror("Failed to allocate memory for result");
    pthread_exit(NULL);
  }
  *result = sum;

  pthread_exit((void*)result);
}

int main() {

  srand(time(NULL));

  int* array = malloc(ARRAY_SIZE * sizeof(int));
  if (array == NULL) {
    perror("Failed to allocate memory for array");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < ARRAY_SIZE; i++) {
    array[i] = i;
  }

  pthread_t threads[NUM_THREADS];
  ThreadData thread_data[NUM_THREADS];

  int chunk = ARRAY_SIZE / NUM_THREADS;

  for (int i = 0; i < NUM_THREADS; i++) {
    thread_data[i].array = array;
    thread_data[i].start = i * chunk;
    thread_data[i].end = (i + 1) * chunk;

    if (i == NUM_THREADS - 1) {
      thread_data[i].end = ARRAY_SIZE;
    }
    else {
      thread_data[i].end = (i + 1) * chunk;
    }

    int rc = pthread_create(&threads[i], NULL, partial_sum, (void*)&thread_data[i]);

    if (rc) {
      fprintf(stderr, "Error: pthread_create() failed with code %d\n", rc);
      free(array);
      exit(EXIT_FAILURE);
    }
  }

  long total_sum = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    void* result;
    
    int rc = pthread_join(threads[i], &result);
    if (rc) {
      fprintf(stderr, "Error: pthread_join() failed with code %d\n", rc);
      free(array);
      exit(EXIT_FAILURE);
    }

    total_sum += *(long *) result;

    free(result);
  }

  printf("The total sum of the arrau is %ld\n", total_sum);

  free(array);

  return 0;
}
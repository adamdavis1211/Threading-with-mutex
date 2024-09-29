#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

int readFile(char[], int[]);
int allocate_array(char file[], int ** values);
void * arraySum(void *);

typedef struct _thread_data_t {
    const int * data;
    int startInd;
    int endInd;
    pthread_mutex_t * lock;
    long long int * totalSum;
} _thread_data_t;

int main(int argc, char * argv[]) {
    if (argc != 3) {
        return -1;
    }
    int * values = NULL;
    int thread_count = atoi(argv[2]);
    int allocate_array_call = allocate_array(argv[1], &values);
    if (thread_count >= allocate_array_call || thread_count < 0) {
        printf("Too many threads requested.\n");
        free(values);
        return -1;
    }
    int read_file_call = readFile(argv[1], values);
    long long int * totalSum = (long long int*)malloc(sizeof(long long int));
    *totalSum = 0;
    struct timeval time_one;
    gettimeofday(&time_one, NULL);
    pthread_mutex_t mutex;
    struct _thread_data_t * thread_data_array = (struct _thread_data_t*)malloc((thread_count) * (sizeof(struct _thread_data_t)));
    int initial_slice_size = allocate_array_call / thread_count;
    int remainder_slice = allocate_array_call % thread_count;
    int slice_size[thread_count];
    for (int i = 0; i < thread_count; i++) {
        slice_size[i] = initial_slice_size;
    }
    for (int i = 0; i < remainder_slice; i++) {
        slice_size[i] += 1;
    }
    int offset = 0;
    for (int i = 0; i < thread_count; i++) {
        thread_data_array[i].data = values;
        thread_data_array[i].lock = &mutex;
        thread_data_array[i].totalSum = totalSum;
        thread_data_array[i].startInd = offset;
        thread_data_array[i].endInd = offset + slice_size[i] - 1;
        offset += slice_size[i];
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, arraySum, (void*) &thread_data_array[i]);
    }
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    struct timeval time_two;
    gettimeofday(&time_two, NULL);
    printf("total time in milliseconds: %li\n", ((time_two.tv_usec - time_one.tv_usec) / 1000));
    printf("total sum: %lli\n", *totalSum);
    pthread_mutex_destroy(&mutex);
    free(totalSum);
    free(thread_data_array);
    free(values);
    pthread_exit(0);
}

int readFile(char file[], int values[]) {
    FILE * file_handler;
    if ((file_handler = fopen(file, "r")) == NULL) {
        printf("File not found...\n");
        return -1;
    }
    int i;
    for (i = 0; values[i] != -1; i++) {
        fscanf(file_handler, "%i", &(values)[i]);
    }
    fclose(file_handler);
    return i;
};

int allocate_array(char file[], int ** values) {
    int amount_of_values = 0;
    char value_of_file[10];
    FILE * file_handler;
    file_handler = fopen(file, "r");
    while (feof(file_handler) == 0) {
        fgets(value_of_file, 10 , file_handler);
        amount_of_values++;
    }
    rewind(file_handler);
    *values = (int *)malloc(amount_of_values * (sizeof(int)));
    for (int i = 0; i < amount_of_values; i++) {
        (*values)[i] = 0;
    }
    (*values)[amount_of_values] = -1;
    return amount_of_values;
}

void * arraySum(void * thread_data_uncasted) {
    long long int threadSum = 0;
    struct _thread_data_t * thread_data = (struct _thread_data_t * )(thread_data_uncasted);
    for (int i = thread_data->startInd; i <= thread_data->endInd; i++) {
        threadSum += thread_data->data[i];
    }
    pthread_mutex_lock(thread_data->lock);
    *(thread_data->totalSum) += threadSum;
    pthread_mutex_unlock(thread_data->lock);
}
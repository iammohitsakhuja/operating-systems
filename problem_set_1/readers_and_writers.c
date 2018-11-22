#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

#define MAX 5
#define NUM_READS 5
#define NUM_WRITES 5

int shared_variable     = 0; /* The shared variable. */
int num_readers_writers = 0; /* Number of readers/writers at any point. Can be either -1 (writer), 0 (none) or +ve. */
int num_waiting_readers = 0; /* Number of readers waiting. Needed to prioritize readers. */

pthread_mutex_t mutex           = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  read_condition  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  write_condition = PTHREAD_COND_INITIALIZER;

void *create_reader(void *arg);
void *create_writer(void *arg);

int main(void)
{
    int tid_readers[MAX];
    int tid_writers[MAX];

    pthread_t readers[MAX];
    pthread_t writers[MAX];

    // Seed the random number generator.
    srandom((unsigned int)time(NULL));

    // Start the writers first.
    for (int i = 0; i < MAX; ++i) {
        tid_writers[i] = i;
        pthread_create(&writers[i], NULL, create_writer, &tid_writers[i]);
    }

    // Start the readers.
    for (int i = 0; i < MAX; ++i) {
        tid_readers[i] = i;
        pthread_create(&readers[i], NULL, create_reader, &tid_readers[i]);
    }

    // Wait for the writers to finish.
    for (int i = 0; i < MAX; ++i) {
        pthread_join(writers[i], NULL);
    }

    // Wait for the readers to finish.
    for (int i = 0; i < MAX; ++i) {
        pthread_join(readers[i], NULL);
    }

    return 0;
}

void *create_reader(void *arg)
{
    int id = *(int *)arg;
    printf("Reader %i created!\n", id);

    // Perform read operation.
    for (int i = 0, num_readers; i < NUM_READS; ++i) {
        // Sleep for a while to ensure that reads and writes do not happen all at once.
        usleep((random() % MAX + MAX) * 1000);

        // Enter the critical section.
        pthread_mutex_lock(&mutex);
        num_waiting_readers++;
        while (num_readers_writers == -1) /* Wait if there are any writers accessing the shared variable. */
            pthread_cond_wait(&read_condition, &mutex);
        --num_waiting_readers;
        num_readers = ++num_readers_writers;
        pthread_mutex_unlock(&mutex);

        // Read the value.
        printf("Reader %i read value '%i' while %i readers were present\n", id, shared_variable, num_readers);

        // Exit the critical section.
        pthread_mutex_lock(&mutex);
        if (--num_readers_writers == 0)
            pthread_cond_signal(&write_condition);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(0);
}

void *create_writer(void *arg)
{
    int id = *(int *)arg;
    printf("Writer %i created!\n", id);

    // Perform write operation.
    for (int i = 0, num_readers; i < NUM_WRITES; ++i) {
        // Sleep for a while to ensure that reads and writes ao not happen all at once.
        usleep((random() % MAX + MAX) * 1000);

        // Enter the critical section.
        pthread_mutex_lock(&mutex);
        while (num_readers_writers != 0) /* Wait if there are any readers or writers accessing the shared variable. */
            pthread_cond_wait(&write_condition, &mutex);
        num_readers = num_readers_writers = -1;
        pthread_mutex_unlock(&mutex);

        // Write the value.
        shared_variable = id;
        printf("Writer %i wrote value '%i' while %i readers were present\n", id, shared_variable, num_readers);

        // Exit the critical section.
        pthread_mutex_lock(&mutex);
        num_readers_writers = 0;
        if (num_waiting_readers > 0) /* If there are any readers waiting for their turn, prioritize them. */
            pthread_cond_broadcast(&read_condition);
        else /* Else, signal a writer. */
            pthread_cond_signal(&write_condition);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(0);
}

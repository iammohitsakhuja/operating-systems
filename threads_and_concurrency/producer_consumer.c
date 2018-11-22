#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

#define BUF_SIZE 3 /* Size of shared buffer. */

int buffer[BUF_SIZE]; /* Shared buffer. */
int tail = 0;         /* Place to add next element to. */
int head = 0;         /* Place to remove next element from. */
int num  = 0;         /* Number elements in buffer. */

pthread_mutex_t mutex         = PTHREAD_MUTEX_INITIALIZER; /* Mutex lock for buffer. */
pthread_cond_t  consumer_cond = PTHREAD_COND_INITIALIZER;  /* Consumer waits on cv. */
pthread_cond_t  producer_cond = PTHREAD_COND_INITIALIZER;  /* Producer waits on cv. */

void *producer(void *param);
void *consumer(void *param);

int main(void)
{
    pthread_t tid1, tid2;

    if (pthread_create(&tid1, NULL, producer, NULL) != 0) {
        fprintf(stderr, "Unable to create producer thread!\n");
        exit(1);
    }

    if (pthread_create(&tid2, NULL, consumer, NULL) != 0) {
        fprintf(stderr, "Unable to create consumer thread!\n");
        exit(2);
    }

    pthread_join(tid1, NULL); /* Wait for producer to exit. */
    pthread_join(tid2, NULL); /* Wait for consumer to exit. */

    puts("Parent quitting");

    return 0;
}

void *producer(void *param)
{
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&mutex); /* Apply lock. */
        if (num > BUF_SIZE)         /* Check for overflow condition. */
            exit(1);
        while (num == BUF_SIZE) /* Block if buffer is full. */
            pthread_cond_wait(&producer_cond, &mutex);
        buffer[tail] = i; /* Buffer not full, so add element. */
        tail         = (tail + 1) % BUF_SIZE;
        num++;
        pthread_mutex_unlock(&mutex); /* Unlock. */

        pthread_cond_signal(&consumer_cond);
        printf("Producer inserted: %i\n", i);
    }

    printf("Producer quitting\n");
    return 0;
}

void *consumer(void *param)
{
    while (true) {
        pthread_mutex_lock(&mutex); /* Apply lock. */
        if (num < 0)                /* Underflow. */
            exit(1);
        while (num == 0) /* Block if buffer is empty. */
            pthread_cond_wait(&consumer_cond, &mutex);
        int i = buffer[head];
        head  = (head + 1) % BUF_SIZE;
        num--;
        pthread_mutex_unlock(&mutex); /* Unlock. */

        pthread_cond_signal(&producer_cond);
        printf("Consumer value: %i\n", i);
    }
}

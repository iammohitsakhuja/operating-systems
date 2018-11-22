#include <pthread.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

#define NUM_THREADS 4

void *hello(void *arg)
{
    printf("Hello thread!\n");
    return 0;
}

int main(void)
{
    pthread_t tid[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_create(&tid[i], NULL, hello, NULL);

    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_join(tid[i], NULL);

    return 0;
}

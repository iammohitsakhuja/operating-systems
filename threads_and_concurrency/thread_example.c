#include <pthread.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

void *foo(void *arg)
{
    printf("Foobar!\n");
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_create(&tid, &attr, foo, NULL);

    pthread_join(tid, NULL);

    return 0;
}

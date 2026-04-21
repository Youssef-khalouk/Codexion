
#include <pthread.h>
#include <stdio.h>
#include <time.h>

pthread_mutex_t lock;
pthread_cond_t cond;

int counter = 0;

int mutix = 1;

void* add1()
{
    int mu = mutix;
    mutix++;
    while (1)
    {
        pthread_mutex_lock(&lock);
        if (counter >= 100)
        {
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&cond);
            break;
        }
        counter++;
        printf("worker number %d adding one, counter is %d\n", mu, counter);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* add2(void* workder_id)
{
    int mu = *((int*)workder_id);
    while (1)
    {
        pthread_mutex_lock(&lock);
        while (counter < 100)
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1; // wait 1 second from now
            int a = pthread_cond_timedwait(&cond, &lock, &ts);
            if (a)
            {
                printf("worker %d is timeout.\n", mu);
            }
        }
        if (counter >= 200)
        {
            pthread_mutex_unlock(&lock);
            printf("worker %d is done.\n", mu);
            break;
        }
        counter++;
        printf("worker number %d adding one, counter is %d\n", mu, counter);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main()
{

    pthread_t worker1;
    pthread_t worker2;
    pthread_t worker3;
    pthread_t worker4;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    int worker_id1 = 1;
    pthread_create(&worker1, NULL, (void*)add1, &worker_id1);
    int worker_id2 = 2;
    pthread_create(&worker2, NULL, (void*)add2, &worker_id2);
    int worker_id3 = 3;
    pthread_create(&worker3, NULL, (void*)add2, &worker_id3);
    int worker_id4 = 4;
    pthread_create(&worker4, NULL, (void*)add2, &worker_id4);


    pthread_join(worker1, NULL);
    pthread_join(worker2, NULL);
    pthread_join(worker3, NULL);
    pthread_join(worker4, NULL);


    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    printf("counter = %d\n", counter);
    printf("Done.\n");

    return 0;
}
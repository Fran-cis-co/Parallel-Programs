#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

const int MAX_STRING = 99;
const int MAX_THREADS = 2000;

long thread_count;
int msg = 0;
char *message;
pthread_mutex_t mutex;

void Get_args(int argc, char *argv[]);

/* Thread function */
void *Thread_work(void *rank);

/*-----------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    long thread;
    pthread_t *thread_handles;
    message = malloc(MAX_STRING * sizeof(char));

    Get_args(argc, argv);
    /* allocate array for threads */
    thread_handles = malloc(thread_count * sizeof(pthread_t));

    /* initialize mutex */
    pthread_mutex_init(&mutex, NULL);

    /* start threads */
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Thread_work,
                       (void *)thread);
    }

    /* wait for threads to complete */
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }
    pthread_mutex_destroy(&mutex);
    free(message);
    free(thread_handles);

    return 0;
}
/*-------------------------------------------------------------------
 * Function:    Thread_work
 * Purpose:     Producer rank 1: create msg
 *		Consumer rank 0: print out msg
 * In arg:      rank
 * Global var:  mutex, msg, message
 */

void *Thread_work(void *rank)
{
    long my_rank = (long)rank;

    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (my_rank % 2 != 0)
        {
            if (msg)
            {
                printf("Th %ld > message : %s\n",
                       my_rank, message);
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
        else
        {
            sprintf(message, "Hello from %ld", my_rank);
            // printf("Th %ld > created message\n", my_rank);
            msg = 1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void Get_args(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
} /* Get_args */

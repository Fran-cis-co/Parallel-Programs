#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

const int MAX_STRING = 99;
const int MAX_THREADS = 2000;

long thread_count;
int msg = 0;
int receiver = 0;
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

        // if there is no message ready to be recieved
        if (!msg)
        {
            // Check if the thread is ready to produce a message
            if (my_rank == receiver)
            {
                sprintf(message, "Hello from %ld", my_rank);
                // set msg to 1 to let program know a message is ready to be consumed
                msg = 1;
            }
        }

        // if there is a message ready to be consumed and the thread is ahead of the thread which produced the message
        if (msg && my_rank == (receiver + 1) % thread_count)
        {
            printf("Th %ld > Received : %s\n",
                   my_rank, message);
            // set msg to 0 to let program know there is no message ready
            msg = 0;
            // set the reciever to the thread ahead
            receiver = (receiver + 1) % thread_count;
        }

        pthread_mutex_unlock(&mutex);
        // once the program has reached the final thread and there is no message available exit the program
        if (receiver == 0 && msg == 0)
        {
            break;
        }
    }

    return NULL;
}

void Get_args(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
} /* Get_args */

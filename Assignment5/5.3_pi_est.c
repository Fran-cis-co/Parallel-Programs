#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

const int MAX_THREADS = 1024;

long thread_count;
pthread_mutex_t mutex;
long long int number_of_tosses;
long long int number_in_circle;
double pi_estimate;

void Usage(char *progname);
void Get_args(int argc, char *argv[]);
void *Thread_work(void *rank);

int main(int argc, char *argv[])
{
    long thread;
    pthread_t *thread_handles;

    Get_args(argc, argv);
    thread_handles = malloc(thread_count * sizeof(pthread_t));

    pthread_mutex_init(&mutex, NULL);

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Thread_work,
                       (void *)thread);
    }

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }

    pthread_mutex_destroy(&mutex);

    pi_estimate = 4 * number_in_circle / ((double)number_of_tosses);

    printf("Pi estimate: \n");
    printf("%lf", pi_estimate);

    free(thread_handles);

    return 0;
}

void *Thread_work(void *rank)
{
    long my_rank = (long)rank;
    long long toss;
    long long my_n = number_of_tosses / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;
    srand(time(NULL) + my_rank);

    for (toss = my_first_i; toss < my_last_i; toss++)
    {

        pthread_mutex_lock(&mutex);
        double x = (2.0 * rand() / RAND_MAX) - 1.0;
        double y = (2.0 * rand() / RAND_MAX) - 1.0;
        double distance_squared = (x * x) + (y * y);
        if (distance_squared <= 1)
            number_in_circle++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void Usage(char *progname)
{
    fprintf(stderr, "Usage: %s \n", progname);
    exit(0);
}

void Get_args(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS)
        Usage(argv[0]);
    number_of_tosses = strtoll(argv[2], NULL, 10);
    if (number_of_tosses <= 0)
        Usage(argv[0]);
}
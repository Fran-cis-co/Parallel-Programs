#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

const int MAX_THREADS = 1024;

long thread_count;
pthread_mutex_t mutex;
long long n;
double *vector;
double *matrix;
double *result;

void Usage(char *progname);
void Get_args(int argc, char *argv[]);
void *Thread_work(void *rank);

int main(int argc, char *argv[])
{
    long thread;
    pthread_t *thread_handles;

    Get_args(argc, argv);
    thread_handles = malloc(thread_count * sizeof(pthread_t));

    vector = malloc(thread_count * sizeof(double));
    matrix = malloc(thread_count * thread_count * sizeof(double));
    result = malloc(thread_count * sizeof(double));

    printf("Enter vector: (size of n)\n");
    for (int i = 0; i < n; i++)
    {
        scanf("%lf", &vector[i]);
    }

    printf("Enter the matrix: (size of n^2)\n");
    for (int i = 0; i < n * n; i++)
    {
        scanf("%lf", &matrix[i]);
    }

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

    printf("Result: \n");
    for (int i = 0; i < n; i++)
    {
        printf("%lf ", result[i]);
    }

    free(vector);
    free(matrix);
    free(result);
    free(thread_handles);

    return 0;
}

void *Thread_work(void *rank)
{
    long my_rank = (long)rank;
    long long i;
    long long j;
    long long my_n = n / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    for (i = my_first_i; i < my_last_i; i++)
    {
        double local_result = 0;
        for (j = 0; j < n; j++)
        {
            pthread_mutex_lock(&mutex);
            local_result += matrix[i * n + j] * vector[j];
            pthread_mutex_unlock(&mutex);
        }
        result[my_rank] = local_result;
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
    n = strtoll(argv[2], NULL, 10);
    if (n <= 0)
        Usage(argv[0]);
}
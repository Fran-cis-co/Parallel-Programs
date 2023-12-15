#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

const int MAX_THREADS = 1024;

long thread_count;
pthread_mutex_t mutex;
// *--- create these as global variables to be used across threads ---* //
int n;
double a, b;
int data_count;
double *data;
float *bin_maxes;
int *bin_counts;
// *------* //

void Usage(char *progname);
void Get_args(int argc, char *argv[]);
void *Thread_work(void *rank);

int main(int argc, char *argv[])
{
    long thread;
    pthread_t *thread_handles;

    Get_args(argc, argv);
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    srand(time(NULL));

    // *--- Allocate memory ---* //
    bin_maxes = malloc(n * sizeof(float));
    bin_counts = malloc(n * sizeof(int));
    data = malloc(data_count * sizeof(double));
    // *------ *//

    // *--- Create randomly generated data depending on the data count size ---*//
    float distance = b - a;

    printf("Rnadomly Generated Data: \n");
    for (int i = 0; i < data_count; i++)
    {
        data[i] = (double)rand() / (double)RAND_MAX * (distance + a);
        printf("%f ", data[i]);
    }
    printf("\n");
    // * ------- * //

    // *--- Create the bins ---* //
    double intervalOfBin = (b - a) / n;
    for (int i = 0; i < n; i++)
    {
        bin_maxes[i] = intervalOfBin * (float)(i + 1) + a;
        bin_counts[i] = 0;
    }
    // *------* //

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

    for (int i = 0; i < n; i++)
    {
        if (i == 0)
        {
            printf("[%10.2f-%10.2f] : ", a, bin_maxes[i]);
        }
        else
        {
            printf("[%10.2f-%10.2f] : ", bin_maxes[i - 1], bin_maxes[i]);
        }

        printf("%d", bin_counts[i]);
        printf("\n");
    }

    free(thread_handles);
    free(bin_maxes);
    free(bin_counts);
    free(data);

    return 0;
}

void *Thread_work(void *rank)
{
    long my_rank = (long)rank;
    long long my_n = data_count / thread_count;
    long long my_first_i = my_n * my_rank;
    // long long my_last_i = my_first_i + my_n;
    long long my_last_i = (my_rank == thread_count - 1) ? data_count : my_first_i + my_n;
    // int correctBin;

    // *--- Assign the bins ---* //
    for (int i = my_first_i; i < my_last_i; i++)
    {
        pthread_mutex_lock(&mutex);
        int correctIndex = 0;
        while (data[i] > bin_maxes[correctIndex])
        {
            correctIndex++;
        }
        bin_counts[correctIndex]++;
        pthread_mutex_unlock(&mutex);
    }
    // *------* //
    return NULL;
}

void Usage(char *progname)
{
    fprintf(stderr, "Usage: %s \n", progname);
    printf("\n [programname] [thread count] [lowerbound] [upwardbound] [data count] [bin count]");
    exit(0);
}

void Get_args(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS)
        Usage(argv[0]);
    a = strtoll(argv[2], NULL, 10);
    b = strtoll(argv[3], NULL, 10);
    data_count = strtoll(argv[4], NULL, 10);
    n = strtoll(argv[5], NULL, 10);
}
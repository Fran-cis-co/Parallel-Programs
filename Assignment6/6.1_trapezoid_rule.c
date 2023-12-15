#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

const int MAX_THREADS = 1024;

long thread_count;
pthread_mutex_t mutex;
long long n;
double a, b;
double total_int;

void Usage(char *progname);
void Get_args(int argc, char *argv[]);
void *Thread_work(void *rank);
double f(double x);
double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);

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

    printf("With n = %lld trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.15e\n", a, b, total_int);

    free(thread_handles);

    return 0;
}

double Trap(double left_endpt, double right_endpt, int trap_count, double base_len)
{
    double estimate, x;
    int i;

    // calculate the integral of the local trapezoid with hard coded function (x^2)
    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    // calculate the area in the current iterated process
    for (i = 1; i <= trap_count - 1; i++)
    {
        x = left_endpt + i * base_len;
        estimate += f(x);
    }
    estimate = estimate * base_len;
    return estimate;
}

double f(double x)
{
    return x * x;
}

void *Thread_work(void *rank)
{
    long my_rank = (long)rank;
    long long i;
    long long my_n = n / thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;
    double h = (b - a) / n;
    double my_a = a + my_rank * my_n * h;
    double my_b = my_a + my_n * h;
    double my_int;

    my_int = Trap(my_a, my_b, my_n, h);

    pthread_mutex_lock(&mutex);
    total_int += my_int;
    pthread_mutex_unlock(&mutex);
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
    a = strtoll(argv[3], NULL, 10);
    b = strtoll(argv[4], NULL, 10);
}
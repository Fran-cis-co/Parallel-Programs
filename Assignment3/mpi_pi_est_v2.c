#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

void Get_tosses(int *tosses_p, int *local_tosses_p, int my_rank, int comm_sz,
                MPI_Comm comm);

double Calculate_num_in_circle(int num_in_circle, int local_tosses, MPI_Comm comm);
int main(void)
{
    int tosses, local_tosses;
    int comm_sz, my_rank;
    double num_in_circle;
    double pi_estimate;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);
    srand(time(NULL) + my_rank);

    Get_tosses(&tosses, &local_tosses, my_rank, comm_sz, comm);

    double local_number_in_circle = 0.0;
    for (int i = 0; i < local_tosses; i++)
    {
        double x = (2.0 * rand() / RAND_MAX) - 1.0;
        double y = (2.0 * rand() / RAND_MAX) - 1.0;
        double distance_squared = (x * x) + (y * y);
        if (distance_squared <= 1)
            local_number_in_circle++;
    }

    MPI_Allreduce(&local_number_in_circle, &num_in_circle, 1, MPI_DOUBLE, MPI_SUM, comm);

    if (my_rank == 0)
    {
        pi_estimate = 4 * num_in_circle / ((double)tosses);
        printf("%lf", pi_estimate);
    }

    MPI_Finalize();
    return 0;
}

void Get_tosses(int *tosses_p, int *local_tosses_p, int my_rank, int comm_sz,
                MPI_Comm comm)
{
    // get input of n inside process 0
    if (my_rank == 0)
    {
        printf("Enter the number of tosses \n");
        scanf("%d", tosses_p);
    }

    /*
     *   Do MPI_Bcast on n
     *   This saves n and lets each process know of the variable
     */
    MPI_Bcast(tosses_p, 1, MPI_INT, 0, comm);
    // set local_n to know how many tosses each process gets
    *local_tosses_p = *tosses_p / comm_sz;
}
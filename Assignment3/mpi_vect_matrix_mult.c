#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);

void Print_result(double result[], int n, int my_rank, int comm_sz, MPI_Comm comm);
int main(void)
{
    int n, local_n;
    int comm_sz, my_rank;
    double *local_matrix;
    double *local_result;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);
    srand(time(NULL));

    Read_n(&n, &local_n, my_rank, comm_sz, comm);

    // allocate memory
    local_matrix = (double *)malloc(n * local_n * sizeof(double));
    local_result = (double *)malloc(local_n * sizeof(double));
    double vector[n];
    double result[n];
    int i;
    int j;
    double *a = NULL;

    // get the vector and matrix
    if (my_rank == 0)
    {
        printf("Enter vector: (size n)\n");
        for (i = 0; i < n; i++)
        {
            scanf("%lf", &vector[i]);
        }

        a = (double *)malloc(n * n * sizeof(double));
        printf("\nEnter the matrix: (n x n)\n");
        for (i = 0; i < n * n; i++)
        {
            scanf("%lf", &a[i]);
        }
    }
    // let all process know of the vector and scatter the total matrix
    MPI_Bcast(vector, n, MPI_DOUBLE, 0, comm);
    MPI_Scatter(a, local_n * n, MPI_DOUBLE, local_matrix, local_n * n, MPI_DOUBLE, 0, comm);

    // compute the vector-matrix multiplcation in each process
    for (i = 0; i < local_n; i++)
    {
        local_result[i] = 0.0;
        for (j = 0; j < n; j++)
        {
            local_result[i] += local_matrix[i * n + j] * vector[j];
        }
    }

    // gather all results under one array
    MPI_Gather(local_result, local_n, MPI_DOUBLE, result, local_n, MPI_DOUBLE, 0, comm);

    // print the results
    if (my_rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            if (i % n == 0)
            {
                printf("\n");
            }
            printf("%lf", result[i]);
        }
    }

    MPI_Finalize();
    return 0;
}

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm)
{
    // get input of n inside process 0
    if (my_rank == 0)
    {
        printf("Enter n (this will make the matrix n x n and the vector size n): \n");
        scanf("%d", n_p);
    }

    /*
     *   Do MPI_Bcast on n
     *   This saves n and lets each process know of the variable
     */
    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
    // set local_n to know how many elements of each vector to send to each process
    *local_n_p = *n_p / comm_sz;
}

// void Print_result(double result[], int n, int my_rank, int comm_sz, MPI_Comm comm)
// {

//     if (my_rank == 0)
//     {
//         for (int i = 0; i < n; i++)
//         {
//             if (i % n == 0)
//             {
//                 printf("\n");
//             }
//             printf("%lf", result[i]);
//         }
//     }
// }

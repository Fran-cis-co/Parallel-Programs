#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

int main(void)
{
    int count = 6;
    int local_array[count], global_array[count];
    int comm_sz, my_rank;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    srand(time(NULL) + my_rank);
    // create array of count elements with random arrays
    for (int i = 0; i < count; i++)
    {
        local_array[i] = rand() % 10;
    }

    // get partial sums inside the process
    MPI_Scan(local_array, global_array, count, MPI_INT, MPI_SUM, comm);

    // print out the original array and partial sums
    printf("Process %d: Array: ", my_rank);
    for (int i = 0; i < count; i++)
    {
        printf("%d ", local_array[i]);
    }
    printf("\n");

    printf("Process %d: Partial Sum Array: ", my_rank);
    for (int i = 0; i < count; i++)
    {
        printf("%d ", global_array[i]);
    }
    printf("\n");

    MPI_Finalize();
    return 0;
}

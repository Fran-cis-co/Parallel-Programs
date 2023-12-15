#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_Vector(double local_vec1[], double local_vec2[], int local_n, int my_rank, int comm_sz, MPI_Comm comm, MPI_Datatype single_double_type);
void Print_vector(double local_vec[], int local_n, int n, char title[], int my_rank, MPI_Comm comm, MPI_Datatype single_double_type);
void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Add_vector(double local_vec1[], double local_vec2[], double local_result[], int local_n);

int main(void)
{
	int n, local_n;
	double *local_vec1, *local_vec2;
	double *x_y_added;
	int comm_sz, my_rank;
	MPI_Comm comm;

	MPI_Init(NULL, NULL);
	comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &comm_sz);
	MPI_Comm_rank(comm, &my_rank);

	MPI_Datatype single_double_type;
	MPI_Type_contiguous(1, MPI_DOUBLE, &single_double_type);
	MPI_Type_commit(&single_double_type);

	Read_n(&n, &local_n, my_rank, comm_sz, comm);
	Read_Vector(local_vec1, local_vec2, local_n, my_rank, comm_sz, comm, single_double_type);

	Print_vector(local_vec1, local_n, n, "x", my_rank, comm, single_double_type);
	Print_vector(local_vec2, local_n, n, "y", my_rank, comm, single_double_type);

	Add_vector(local_vec1, local_vec2, x_y_added, local_n);

	Print_vector(x_y_added, local_n, n, "x + y", my_rank, comm, single_double_type);

	MPI_Finalize();
	return 0;
}

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz, MPI_Comm comm)
{
	if (my_rank == 0)
	{
		printf("Enter n: \n");
		scanf("%d", n_p);
	}

	MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
	*local_n_p = *n_p / comm_sz;
}

void Read_Vector(double local_vec1[], double local_vec2[], int local_n, int my_rank, int comm_sz, MPI_Comm comm, MPI_Datatype single_double_type)
{
	double *x = NULL;
	double *y = NULL;
	int i;

	if (my_rank == 0)
	{
		x = malloc(local_n * comm_sz * sizeof(double));
		y = malloc(local_n * comm_sz * sizeof(double));

		printf("Enter vector x: \n");
		for (i = 0; i < local_n * comm_sz; i++)
			scanf("%lf", &x[i]);
		printf("Enter vector y: \n");
		for (i = 0; i < local_n * comm_sz; i++)
			scanf("%lf", &y[i]);

		MPI_Scatter(x, local_n, single_double_type, local_vec1, local_n, single_double_type, 0, comm);
		MPI_Scatter(y, local_n, single_double_type, local_vec2, local_n, single_double_type, 0, comm);
		free(x);
		free(y);
	}
	else
	{
		MPI_Scatter(x, local_n, single_double_type, local_vec1, local_n, single_double_type, 0, comm);
		MPI_Scatter(y, local_n, single_double_type, local_vec2, local_n, single_double_type, 0, comm);
	}
}

void Print_vector(double local_vec[], int local_n, int n, char title[], int my_rank, MPI_Comm comm, MPI_Datatype single_double_type)
{
	double *a = NULL;
	int i;

	if (my_rank == 0)
	{
		a = malloc(n * sizeof(double));
		// acquire vector data to print out
		MPI_Gather(local_vec, local_n, single_double_type, a, local_n, single_double_type, 0, comm);
		printf("Vector %s: \n", title);
		for (i = 0; i < n; i++)
			printf("%f ", a[i]);
		printf("\n");
		free(a);
	}
	else
	{
		// Acquire all vector data until it reaches process 0
		MPI_Gather(local_vec, local_n, single_double_type, a, local_n, single_double_type, 0, comm);
	}
}

void Add_vector(double local_vec1[], double local_vec2[], double local_result[], int local_n)
{

	int i;
	for (i = 0; i < local_n; i++)
	{
		local_result[i] = local_vec1[i] + local_vec2[i];
	}
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 

double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);
double f(double x);
void Read_data(double* a_p, double* b_p, int* n_p, int my_rank, MPI_Comm comm);
double Calculate_integral(double total_int, double local_int, int source, int comm_sz, MPI_Comm comm);

int main(void) 
{
	// initialize variables
	int my_rank, comm_sz, n, local_n;
	double a, b, h, local_a, local_b;
	double local_int, total_int;
	int source;
	MPI_Comm comm;

	MPI_Init(NULL, NULL);
	comm = MPI_COMM_WORLD;
	MPI_Comm_rank(comm, &my_rank);
	MPI_Comm_size(comm, &comm_sz);

	// read data
	Read_data(&a, &b, &n, my_rank, comm);
	
	// calculate the height of each subinterval
	h = (b - a) / n;
	// create the amount of subintervals each process will take
	local_n = n / comm_sz;

	// create the lower bound and upperbound for each process
	local_a = a + my_rank * local_n * h;
	local_b = local_a + local_n * h;
	// create the local trapezoids
	local_int = Trap(local_a, local_b, local_n, h);

	// add up all the areas from each process
	total_int = Calculate_integral(total_int, local_int, source, comm_sz, comm);

	// Commented out code of the original method using send and recv
	/**
	if (my_rank != 0)
	{
		MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, comm);
	}
	else
	{
		total_int = local_int;
		for (source = 1; source < comm_sz; source++)
		{	
			MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, comm, MPI_STATUS_IGNORE);
			total_int += local_int;
		}
	}

	*/

	if (my_rank == 0)
	{
		printf("With n = %d trapezoids, our estimate\n", n);
		printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
	}
	MPI_Finalize();
	return 0;
}

void Read_data(double* a_p, double* b_p, int* n_p, int my_rank, MPI_Comm comm)
{
	// get data for a, b, and subintervals (N)
	if (my_rank == 0)
	{
		printf("Enter the lower bound: \n");
		scanf("%lf", a_p);

		printf("Enther the upper bound: \n");
		scanf("%lf", b_p);

		printf("Enther the number of subintervals: \n");
		scanf("%d", n_p);
	}

	// use Bcast to send these to each process
	MPI_Bcast(a_p, 1, MPI_DOUBLE, 0, comm);
	MPI_Bcast(b_p, 1, MPI_DOUBLE, 0, comm);
	MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
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

// for this program the function will be x^2
double f(double x)
{
	return x * x;
}



double Calculate_integral(double total_int, double local_int, int source, int comm_sz, MPI_Comm comm)
{
	total_int = local_int;
	// add up trapezoid areas of the process to the total int
	for (source = 1; source < comm_sz; source++)
	{
		total_int += local_int;
	}
	// use MPI_Reduce to send to the total_int variable which will eventually add up all the areas that is contained in each process
	MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
	return total_int;
}
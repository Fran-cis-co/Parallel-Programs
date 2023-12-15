#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Print_Array(int array[], int n);
int Partial_Sum(int array[], int n);

int main(void)
{
	const int n = 5;
	int array[n];
	int partial_sum_array[n];
	srand(time(NULL));

	for (int i = 0; i < n; i++)
	{
		array[i] = rand() % 5;
	}

	Print_Array(array, n);

	for (int i = 0; i < n; i++)
	{
		partial_sum_array[i] = Partial_Sum(array, i);
	}

	Print_Array(partial_sum_array, n);

	return 0;
}

int Partial_Sum(int array[], int n)
{
	int sum = 0;
	for (int i = 0; i <= n; i++)
	{
		sum = sum + array[i];
	}
	return sum;
}

void Print_Array(int array[], int n)
{
	for (int i = 0; i < n; i++)
	{
		printf(" %d", array[i]);
	}
	printf("\n");
}
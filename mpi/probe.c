#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MAX_RAND 10

int main (int argc, char * argv[])
{
  int rank, size;

  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);  /* get number of processes */
  
  int number_amount;
  if (rank == 0) {
	const int MAX_MSG_SIZE = 100;
	int numbers[MAX_MSG_SIZE];

	srand(time(NULL));
	number_amount = (rand() / (float)RAND_MAX) * MAX_MSG_SIZE;

	MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
	printf("0 sent %d numbers to 1\n", number_amount);
  } else if (rank == 1) {
	MPI_Status status;
	MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

	MPI_Get_count(&status, MPI_INT, &number_amount);

	int* number_buf = (int*) malloc(sizeof(int) * number_amount);

	MPI_Recv(number_buf, number_amount, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	printf("Process no 1 received %d numbers\n", number_amount);  
  	
	free(number_buf);
  }

  MPI_Finalize();
  return 0;
}

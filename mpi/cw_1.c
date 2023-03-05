#include <stdio.h>
#include <mpi.h>

int main (int argc, char * argv[])
{
  int rank, size;
  int number;

  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);  /* get number of processes */
  if (rank == 0) {
	number = 2137;
	MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } else if (rank == 1) {
	MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	printf("Process 1 received number %d from process 0\n", number);
  }
  MPI_Finalize();
  return 0;
}

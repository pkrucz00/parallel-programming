#include <stdio.h>
#include <mpi.h>

#define PING_PONG_LIMIT 10

int main (int argc, char * argv[])
{
  int rank, size;

  int token;

  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);  /* get number of processes */
  if (rank == 0) {
	token = 2137;
  } else if (rank != 0) {
	MPI_Recv(&token, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	printf("Process no %d received token %d\n", rank, token);  
  }
   MPI_Send(&token, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);
  if (rank == 0) {
  	MPI_Recv(&token, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("The initial process received token %d\n", token);
  }
 
  MPI_Finalize();
  return 0;
}

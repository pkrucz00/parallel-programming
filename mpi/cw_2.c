#include <stdio.h>
#include <mpi.h>

#define PING_PONG_LIMIT 2000

int main (int argc, char * argv[])
{
  int rank, size;

  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);  /* get number of processes */
  int count;
  int partner_rank = (rank + 1) % 2;

  double t1, t2, total;
  t1 = MPI_Wtime();
 
  while (count < PING_PONG_LIMIT) {
  	if (rank == count % 2) {
		count++;
		MPI_Ssend(&count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
                // printf("send %d -> %d: %d\n", rank, partner_rank, count);
	} else {
		MPI_Recv(&count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// printf("recv %d -> %d: %d\n", rank, partner_rank, count);
	}
  }
  t2 = MPI_Wtime();
  total = t2 - t1;
  printf("Communication time of %d messages: %f [s]\n", PING_PONG_LIMIT / 2, total);

  MPI_Finalize();
  return 0;
}

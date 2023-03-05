#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MAX_RAND 10
#define DEFAULT_TAG 0

void init_mpi(int* argc, char** argv[], int* rank, int* size){
   MPI_Init (argc, argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, size);  /* get number of processes */
}

void my_MPI_send(int* number_buf, int number_amount, int receiver) {
  MPI_Send(number_buf, number_amount, MPI_INT, receiver, DEFAULT_TAG, MPI_COMM_WORLD);
}


void my_MPI_receive(int* number_buf, int number_amount, int sender){
  MPI_Recv(number_buf, number_amount, MPI_INT, sender, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

int main (int argc, char * argv[])
{
  int rank, size;
  init_mpi(&argc, &argv, &rank, &size);
 
  int number_amount = 1;
  int* number_buf = (int*) malloc(sizeof(int) * number_amount);
  int i; 

  if (rank == 0) {
        for (i = 0; i < number_amount; i++) {
  	    number_buf[i] = -1;
        }

	my_MPI_send(number_buf, number_amount, 1);
	printf("0 sent %d numbers to 1\n", number_amount);
  } else if (rank == 1) {
  	my_MPI_receive(number_buf, number_amount, 0);
        for (i = 0; i < number_amount; i++) {
  		printf("%d\n", number_buf[i]);
  	}

  }

  free(number_buf);
  MPI_Finalize();
  return 0;
}

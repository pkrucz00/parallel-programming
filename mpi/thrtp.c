#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 1001
#define DEFAULT_TAG 0

void init_mpi(int* argc, char** argv[], int* rank, int* size){
   MPI_Init (argc, argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, size);  /* get number of processes */
}

void my_MPI_send(int* number_buf, int number_amount, int receiver) {
  MPI_Send(number_buf, number_amount, MPI_INT, receiver, DEFAULT_TAG, MPI_COMM_WORLD);
//	printf("Sent %d numbers to %d\n", number_amount, receiver);
}


void my_MPI_receive(int* number_buf, int number_amount, int sender){
  MPI_Recv(number_buf, number_amount, MPI_INT, sender, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  // int i;
  // printf("From %d with love :*\n", sender); 
  //      for (i = 0; i < number_amount; i++) {
  //		printf("%d\n", number_buf[i]);
  //	}

}

void MPI_one_ping_pong(int rank, int* number_buf, int number_amount) {
  if (rank == 0) {
  	my_MPI_send(number_buf, number_amount, 1);
        my_MPI_receive(number_buf, number_amount, 1);
  } else if (rank == 1) {
        my_MPI_receive(number_buf, number_amount, 0); 
  	my_MPI_send(number_buf, number_amount, 0);
  }
}

int* init_buf(int n){
  int* buf = (int*) malloc(sizeof(int) * n);
  int i;
  for (i=0; i < n; i++){
     buf[i] = -1;
  }
  return buf;
}

float measure_bandwidth(int rank, int number_amount) {
  
  int i;
  double t1, t2;
  int* number_buf = init_buf(number_amount);

  MPI_Barrier(MPI_COMM_WORLD);
  t1 = MPI_Wtime();
  for (i = 0; i < N; i++){
     MPI_one_ping_pong(rank, number_buf, number_amount);    
  } 
  t2 = MPI_Wtime();

  int buff_size = sizeof(int)*number_amount;
  double time = (t2 - t1)/((float) N);
  
  double thrtp = (double) (8*buff_size)/(1000000.0*time);

  free(number_buf);
  
  return thrtp;
}

int main (int argc, char * argv[])
{
  int rank, size;
  init_mpi(&argc, &argv, &rank, &size);
 
  int number_amount = 1000000;
  double thrtp = measure_bandwidth(rank, number_amount);
  if (rank == 0) printf("Throughtput: %f [Mbit/s]\n", thrtp);

  MPI_Finalize();
  return 0;
}

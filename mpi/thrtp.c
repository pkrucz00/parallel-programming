#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 1001
#define DEFAULT_TAG 0
#define TEST_CASES_NUM 12
#define CSV_NAME "results/normal_1_node.csv"
#define BUFFERED 1

void init_mpi(int* argc, char** argv[], int* rank, int* size){
   MPI_Init (argc, argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, size);  /* get number of processes */
}

void my_MPI_send(int* number_buf, int number_amount, int receiver) {
  # ifdef BUFFERED
   int buffer_attached_size = MPI_BSEND_OVERHEAD +  number_amount*sizeof(int);
   char* buffer_attached = (char*) malloc(buffer_attached_size);
   MPI_Buffer_attach(buffer_attached, buffer_attached_size);

   MPI_Bsend(number_buf, number_amount, MPI_INT, receiver, DEFAULT_TAG, MPI_COMM_WORLD);
        
   MPI_Buffer_detach(&buffer_attached, &buffer_attached_size);
   free(buffer_attached); 
  # else
  MPI_Send(number_buf, number_amount, MPI_INT, receiver, DEFAULT_TAG, MPI_COMM_WORLD);
  # endif /* BUFFERED */

}


void my_MPI_receive(int* number_buf, int number_amount, int sender){
  MPI_Recv(number_buf, number_amount, MPI_INT, sender, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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

double measure_ping_pong_time(int rank, int number_amount) {
  
  int i;
  double t1, t2;
  int* number_buf = init_buf(number_amount);

  MPI_Barrier(MPI_COMM_WORLD);
  t1 = MPI_Wtime();
  for (i = 0; i < N; i++){
     MPI_one_ping_pong(rank, number_buf, number_amount);    
  } 
  t2 = MPI_Wtime();

  free(number_buf);
  return (t2 - t1)/((double) N);
}

double compute_thrtp(double time_in_sec, int number_amount) {
  int buff_size = sizeof(int)*number_amount;
  return (double) (8*buff_size)/(1000000.0*time_in_sec);
}

void export_to_csv(int* buff_size, double* throughtputs) {
  printf("Message_size, Throughtput\n");
  
  int i;
  for (i = 0; i < TEST_CASES_NUM; i++) {
  	printf( "%d, %f\n", buff_size[i], throughtputs[i]);
  }
}

int main (int argc, char * argv[])
{
  int rank, size;
  init_mpi(&argc, &argv, &rank, &size);
 
  const int number_amounts[TEST_CASES_NUM] = {100, 250, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000};
  double* thrtps = (double*) malloc(sizeof(double)*TEST_CASES_NUM);
  int* buff_size = (int*) malloc(sizeof(int)*TEST_CASES_NUM);
  int i;  
   for (i = 0; i < TEST_CASES_NUM; i++) {
 	double snd_rcv_time_sec = measure_ping_pong_time(rank, number_amounts[i]);
  	buff_size[i] = sizeof(int)*number_amounts[i];
        thrtps[i] = compute_thrtp(snd_rcv_time_sec, number_amounts[i]); 
  } 
  // double delay = 1000.0 * measure_ping_pong_time(rank, 1);  

  if (rank == 0) {
  	export_to_csv(buff_size, thrtps);
       //  printf("%f\n", delay);
  	
  }
  free(thrtps);
  free(buff_size);

  MPI_Finalize();

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>

#define N 100
#define DEFAULT_TAG 0
#define TEST_CASES_NUM 19
#define CSV_NAME "results/normal_1_node.csv"
// #define BUFFERED 1

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

double* init_double_table(int n){
   return (double*) malloc(sizeof(double) * n);
}

double avg(double* nums, int n){
   int i;
   double sum = 0;
   for (i = 0; i < n; i++){
       sum += nums[i];
   }
   return sum / (double) n;
}

double std(double* nums, double mean, int n){
   double result = 0;
   int i;
   for (i = 0; i < n; i++){
      result += (nums[i] - mean)*(nums[i] - mean);
   }   

   return (double) sqrt(result / (double) (n - 1));  
}

void measure_ping_pong_times(double* times, int rank, int number_amount) {
  
  int i;
  double t1, t2;
  int* number_buf = init_buf(number_amount);

  MPI_Barrier(MPI_COMM_WORLD);
  
  for (i = 0; i < N; i++){
     t1 = MPI_Wtime();
     MPI_one_ping_pong(rank, number_buf, number_amount);    
     t2 = MPI_Wtime();
     times[i] = t2 - t1;
  } 
   
  free(number_buf);
}

double compute_thrtp(double time_in_sec, int number_amount) {
  int buff_size = sizeof(int)*number_amount;
  return (double) (8*buff_size)/(1000000.0*time_in_sec);
}

double* compute_thrtp_measures(double* results, double* times_in_sec, int number_of_measures, int number_amount){
  int i;
  for (i = 0; i < number_of_measures; i++){
     results[i] = compute_thrtp(times_in_sec[i], number_amount);
  }
  return results;
}

void export_to_csv(int* buff_size, double* throughtputs, double* stds) {
  printf("Message_size, Throughtput, Standard deviation\n");
  
  int i;
  for (i = 0; i < TEST_CASES_NUM; i++) {
	printf( "%d, %f, %f\n", buff_size[i], throughtputs[i], stds[i]);
  }
}

int main (int argc, char * argv[])
{
  int rank, size;
  init_mpi(&argc, &argv, &rank, &size);
 
  const int number_amounts[TEST_CASES_NUM] = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000, 10000000};
  
  double* thrtps = (double*) malloc(sizeof(double)*TEST_CASES_NUM);
  double* stds = (double*) malloc(sizeof(double)*TEST_CASES_NUM);

  int* buff_size = (int*) malloc(sizeof(int)*TEST_CASES_NUM);
  double* thrtp_measures = (double*) malloc(sizeof(double)*N);
  double* time_measures = (double*) malloc(sizeof(double)*N);
  int i;  
   for (i = 0; i < TEST_CASES_NUM; i++) {
 	measure_ping_pong_times(time_measures, rank, number_amounts[i]);
        compute_thrtp_measures(thrtp_measures, time_measures, N, number_amounts[i]);
        buff_size[i] = sizeof(int)*number_amounts[i];
        
       thrtps[i] = avg(thrtp_measures, N); 
        stds[i] = std(thrtp_measures, thrtps[i], N);
  } 
  // double delay = 1000.0 * measure_ping_pong_time(rank, 1);  

  if (rank == 0) {
  export_to_csv(buff_size, thrtps, stds);
       //  printf("%f\n", delay);
  	
  }  
  free(thrtps);
  free(stds);
  free(buff_size);

  free(time_measures);
  free(thrtp_measures);
  
MPI_Finalize();
 
  return 0;
}

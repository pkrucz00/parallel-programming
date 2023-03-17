#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#ifndef N
#define N 100000000
#endif

#define SCALING "strong"

#ifndef TEST_CASE_NUM
#define TEST_CASE_NUM 0
#endif

typedef struct Point {
  float x;
  float y;
} Point;

void init_mpi(int* argc, char** argv[], int* rank, int* size){
   MPI_Init (argc, argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, size);  /* get number of processes */
}

int point_in_circle() {
   Point p;
   p.x = (float) rand() / (float) RAND_MAX;
   p.y = (float) rand() / (float) RAND_MAX;
  
  return (p.x*p.x + p.y*p.y) < 1;  
}

long long draw_points_in_circle(long long n) {
  long long i;
  long long sum = 0;
  for ( i = 0; i < n; i++){
       sum += point_in_circle();
  }
   return sum;
}

float approx_pi(long long sum){
  double proportion = (double) sum / (double) N;
  return 4*proportion;
}

int main (int argc, char * argv[])
{
  int rank, size;
  
  init_mpi(&argc, &argv, &rank, &size);

  srand(time(NULL) + rank);
   
  long long n_per_process = N / size;
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  double t1 = MPI_Wtime();
  long long local_sum = draw_points_in_circle(n_per_process);

  long long global_sum;
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  double t2 = MPI_Wtime();
  double total_time = t2 - t1;

  if (rank == 0){
  	double pi = approx_pi(global_sum);
  	printf("%f,%f,%d,%s,%d\n", pi, total_time, N, SCALING, TEST_CASE_NUM);
  }
  MPI_Finalize();

  return 0;
}

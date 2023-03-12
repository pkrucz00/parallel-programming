#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#define N 1000000000

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

float approx_pi(){
  long long i;
  long long sum = 0;
  for ( i = 0; i < N; i++){
       sum += point_in_circle();
  }
  double proportion = (double) sum / (double) N;
  return 4*proportion;
}

int main (int argc, char * argv[])
{
  int rank, size;
  init_mpi(&argc, &argv, &rank, &size);

  srand(time(NULL) + rank);
  double pi = approx_pi();
  double diff = fabs(pi - M_PI);
   if (rank == 0) {
  	printf("Pi approximation: %f\nDifference from correct: %f\n", pi, diff);
   }
  MPI_Finalize();

  return 0;
}

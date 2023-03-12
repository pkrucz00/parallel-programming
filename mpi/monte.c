#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define N 1000

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
  int sum = 0;
  for ( i = 0; i < N; i++){
       sum += point_in_circle();
  }
  float proportion = (float) sum / (float) N;
  return 4*proportion;
}

int main (int argc, char * argv[])
{
  int rank, size;
  init_mpi(&argc, &argv, &rank, &size);

  srand(time(NULL) + rank);
  float pi = approx_pi();

   if (rank == 0) {
  	printf("Pi approximation: %f\n", pi);
   }
  MPI_Finalize();

  return 0;
}

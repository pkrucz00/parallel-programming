#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#ifndef N
#define N 100000000
#endif /*N*/

#ifndef PROC_NUM
#define PROC_NUM 4
#endif /*PROC_NUM*/

#ifndef PRAGMA_TYPE
#define PRAGMA_TYPE 0
#endif /*PRAGMA_TYPE*/

double* init_array(long long n) {
	return (double*) malloc(n * sizeof(double));
}

int main() {
	omp_set_num_threads(PROC_NUM);
	double* arr = init_array(N);
	long long i;
	unsigned short xi[3];
	short tid;
	
	double t1 = omp_get_wtime();
	#pragma omp parallel private(tid, xi)
	{
		tid = omp_get_thread_num();
		xi[0] = tid;
		xi[1] = tid + 2;
		xi[2] = tid + 1;
	
		#pragma omp for private(i)
		for (i = 0; i < N; i++){
			arr[i] = erand48(xi);
		}

	}
	double t2 = omp_get_wtime();
	printf("%f\n", t2 - t1);
	
	free(arr);
	return 0;
}

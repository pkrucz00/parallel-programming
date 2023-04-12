#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#ifndef N
#define N 15
#endif /*N*/

#ifndef B
#define B 5
#endif /*B*/

#ifndef PROC_NUM
#define PROC_NUM 4
#endif /*PROC_NUM*/

float* init_array(long long n) {
	return (float*) malloc(n * sizeof(double));
}

int asc(const void * a, const void * b) {
	float va = *(const float*) a;
	float vb = *(const float*) b;
	return (va > vb) - (va < vb);
}

int main() {
	omp_set_num_threads(PROC_NUM);
	float* arr = init_array(N);
	float buckets[B][N];
	int bucket_ind[B];
	int i;
	for (i = 0; i < B; i++) {
		bucket_ind[i] = 0;
	}
	
	long long size_of_arr = N * sizeof(float);

	unsigned short xi[3];
	short tid;
	
	double t1 = omp_get_wtime();
	#pragma omp parallel private(tid, xi)
	{
		tid = omp_get_thread_num();
		xi[0] = tid;
		xi[1] = tid + 2;
		xi[2] = tid + 1;

		#pragma omp for private(i) schedule(guided)
		for (i = 0; i < N; i++){
			arr[i] = erand48(xi);
		}
	}

	int number_of_bucket;
	int number_of_element_in_bucket;
	for (i = 0; i < N; i++){
		number_of_bucket = (int) B*arr[i];
		printf("Bucket ind and val   %d %f\n", number_of_bucket, arr[i]);

		number_of_element_in_bucket = bucket_ind[number_of_bucket];
		buckets[number_of_bucket][number_of_element_in_bucket] = arr[i];
		bucket_ind[number_of_bucket]++;		
	}

	for (i = 0; i < B; i++){
		qsort(buckets[i], bucket_ind[i], sizeof(int), asc);
	}

	int j;
	i = 0;
	int bucket_num = 0;
	while (i < N) {
		printf("%d %d\n", bucket_num, bucket_ind[bucket_num]);
		for (j = 0; j < bucket_ind[bucket_num]; j++) {
			arr[i] = buckets[bucket_num][j];
			i++;;
		}
		bucket_num++;
	}  

	for (i = 0; i < N; i++) {
		printf("%f,  ", arr[i]);
	} 
	printf("\n");
	double t2 = omp_get_wtime();
 	 printf("%f,%lld\n", t2 - t1, size_of_arr);
	
	free(arr);
	return 0;
}

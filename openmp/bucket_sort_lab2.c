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

#ifndef SCHEDULE_TYPE 
#define SCHEDULE_TYPE 0
#endif /*SCHEDULE_TYPE*/

#ifndef CHUNK_SIZE
#define CHUNK_SIZE -1
#endif /*CHUNK_SIZE*/

#ifndef TEST_CASE_NUM
#define TEST_CASE_NUM 0
#endif /*TEST_CASE_NUM*/

float* init_array(long long n) {
	return (float*) malloc(n * sizeof(double));
}

void insertion_sort(float arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
 
        /* Move elements of arr[0..i-1], that are
 *           greater than key, to one position ahead
 *                     of their current position */
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
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
	const char* schedule_types[] = { "static", "dynamic", "guided" };	

	unsigned short xi[3];
	short tid;
	
	double t1 = omp_get_wtime();
	#pragma omp parallel private(tid, xi)
	{
		tid = omp_get_thread_num();
		xi[0] = tid;
		xi[1] = tid + 2;
		xi[2] = tid + 1;

		#if SCHEDULE_TYPE == 0	/*static*/
			#if CHUNK_SIZE == -1
				#pragma omp for private(i) schedule(static)
			#else
				#pragma omp for private(i) schedule(static, CHUNK_SIZE)
			#endif /*CHUNK_SIZE*/
		#elif SCHEDULE_TYPE == 1 /*dynamic*/
			#if CHUNK_SIZE == -1
				#pragma omp for private(i) schedule(dynamic)
			#else
				#pragma omp for private(i) schedule(dynamic, CHUNK_SIZE)
			#endif /*CHUNK_SIZE*/
		#elif SCHEDULE_TYPE == 2 /* guided */
			#if CHUNK_SIZE == -1
				#pragma omp for private(i) schedule(guided)
			#else
				#pragma omp for private(i) schedule(guided, CHUNK_SIZE)
			#endif /*CHUNK_SIZE*/
		#endif /* SCHEDULE_TYPE*/
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
		insertion_sort(buckets[i], bucket_ind[i]);
	}

	

	int j;
	for (i = 0; i < B; i++) {
		printf("Bucket %d:  ", i);
		for (j = 0; j < N; j++) {
			printf("%f, ", buckets[i][j]);
		}
		printf("\n"); 
	} 
	double t2 = omp_get_wtime();
 	 printf("%f,%lld,%s,%d,%d\n", t2 - t1, size_of_arr, schedule_types[SCHEDULE_TYPE], CHUNK_SIZE, TEST_CASE_NUM);
	
	free(arr);
	return 0;
}

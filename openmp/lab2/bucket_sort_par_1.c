#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#ifndef N
#define N 150
#endif /*N*/

#ifndef B
#define B 50
#endif /*B*/

#ifndef P
#define P 1
#endif /*P*/

int* init_int_array(long long n){
	return (int*) malloc(n * sizeof(int));
}

float* init_array(long long n) {
	float* result = (float*) malloc(n * sizeof(float));
	int i;
	for (i = 0; i < n; i++){
		result[i] = 0;
	}
	return result;
}

float** init_2d_array(long long m, long long n) {
	float** result = (float**) malloc(n * sizeof(float*));
	int i;
	for (i = 0; i < m; i++){
		result[i] = (float*) malloc(m * sizeof(float));
	}
	return result;
}

void free_2d_array(float** arr, long long m) {
	int i;
	for (i = 0; i < m; i++){
		free(arr[i]);
	}
	free(arr);
}

int* init_ints(int n) {	
	int* result = init_int_array(B);
	
	int i;
	for (i = 0; i < B; i++) {
		result[i] = 0;
	}
	return result;
}

int asc(const void * a, const void * b) {
	float va = *(const float*) a;
	float vb = *(const float*) b;
	return (va > vb) - (va < vb);
}

void fill_array_rand(float* arr, int n) {
	int i;
	unsigned short xi[3];
	unsigned short tid;
	time_t sec = time(NULL);	

	#pragma omp parallel private(tid, xi)
	{
		tid = omp_get_thread_num();
		xi[0] = tid + sec;
		xi[1] = tid + 2 + sec;
		xi[2] = tid + 1 + sec;

		#pragma omp for private(i) schedule(guided)
		for (i = 0; i < n; i++){
			arr[i] = erand48(xi);
		}
	}
}

void distribute_to_buckets(float* arr, float** buckets, int* bucket_ind, int n, int b){
	int i;
	// int i0 =  thr_num * ((float)n / (float)P);
	int number_of_bucket;
	int number_of_elements_in_bucket;
	int i0 = 7;
	i = i0;
	do {
		number_of_bucket = (int) b*arr[i];
		number_of_elements_in_bucket = bucket_ind[number_of_bucket];
		buckets[number_of_bucket][number_of_elements_in_bucket] = arr[i];
		bucket_ind[number_of_bucket]++;

		i = (i + 1) % n;
	} while (i != i0);
}

void sort_buckets(float** buckets, int* bucket_ind, int b) {
	int i;
	for (i = 0; i < b; i++){
		qsort(buckets[i], bucket_ind[i], sizeof(int), asc);
	}
}

void merge_buckets(float* arr, float** buckets, int* bucket_ind, int n){	
	int j;
	int i = 0;
	int bucket_num = 0;
	while (i < n) {
		for (j = 0; j < bucket_ind[bucket_num]; j++) {
			arr[i] = buckets[bucket_num][j];
			i++;;
		}
		bucket_num++;
	}	
}

int* compute_cummulative_sum(int* arr, int n){
	int* cum_arr = init_ints(n);
	int i;
	cum_arr[0] = arr[0];
	for (i = 1; i < n; i++){
		cum_arr[i] = cum_arr[i-1] + arr[i];
	}		
	return cum_arr;
}
 
void print_int_arr(int* arr, int n) {
	int i;
	for (i = 0; i < n; i++) {
		printf("%d, ", arr[i]);
	}
	printf("\n");
}


void print_float_arr(float* arr, int n){
	int i;
	for (i = 0; i < n; i++) {
		printf("%4f, ", arr[i]);
	} 
	printf("\n");
}

int main() {
	omp_set_num_threads(P);

	float* arr = init_array(N);
	float** buckets = init_2d_array(B, N);
	int* bucket_ind = init_ints(B);
	
	long long size_of_arr = N * sizeof(float);

	double t1 = omp_get_wtime();
	// TODO Figure out how to pragma mp this one
	//
	// first sectiion
	fill_array_rand(arr, N);
	//TODO change input parameters 
	distribute_to_buckets(arr, buckets, bucket_ind, N, B);
	sort_buckets(buckets, bucket_ind, B);  
	// end first section
	int* cum_buck_ind = compute_cummulative_sum(bucket_ind, B);
	print_int_arr(cum_buck_ind, B);
	// start second section
	merge_buckets(arr, buckets, bucket_ind, N);
	// sen second section
	double t2 = omp_get_wtime();
	print_float_arr(arr, N);
 	 printf("%f,%lld\n", t2 - t1, size_of_arr);
	
	free(arr);
	free(bucket_ind);
	free(cum_buck_ind);
	free_2d_array(buckets, B);
	return 0;
}

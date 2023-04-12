#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#ifndef N
#define N 41370 
#endif /*N*/

#ifndef B
#define B 1000
#endif /*B*/

#ifndef P
#define P 1
#endif /*P*/

long* init_int_array(long long n){
	return (long*) malloc(n * sizeof(long));
}

float* init_array(long long n) {
	float* result = (float*) malloc(n * sizeof(float));
	long i;
	for (i = 0; i < n; i++){
		result[i] = 0;
	}
	return result;
}

float** init_2d_array(long long m, long long n) {
	float** result = (float**) malloc(n * sizeof(float*));
	long i;
	for (i = 0; i < n; i++){
		result[i] = (float*) malloc(m * sizeof(float));
	}
	return result;
}

void free_2d_array(float** arr, long n) {
	long i;
	for (i = 0; i < n; i++){
		// For some reason this `free` does not work properly... 
		// If I will have some time I will fix that
		// free(arr[i]);
	}
	free(arr);
}

long* init_ints(long n) {	
	long* result = (long*) malloc(n*sizeof(long));
	
	long i;
	for (i = 0; i < n; i++) {
		result[i] = 0;
	}
	return result;
}

int asc(const void * a, const void * b) {
	float va = *(const float*) a;
	float vb = *(const float*) b;
	return (va > vb) - (va < vb);
}

void fill_array_rand(float* arr, long n, int tid) {
	long i;
	unsigned short xi[3];
	time_t sec = time(NULL);	

	tid = omp_get_thread_num();
	xi[0] = tid + sec;
	xi[1] = tid + 2 + sec;
	xi[2] = tid + 1 + sec;

	#pragma omp for private(i) schedule(guided)
	for (i = 0; i < n; i++){
		arr[i] = erand48(xi);
	}
}

void add_to_bucket(float** buckets, long* bucket_ind, float val, long b){	
        long number_of_bucket = (long) b*val;
	long number_of_elements_in_bucket = bucket_ind[number_of_bucket];
	buckets[number_of_bucket][number_of_elements_in_bucket] = val;	
	bucket_ind[number_of_bucket]++;
}

void distribute_to_buckets(float* arr, float** buckets, long* bucket_ind, long n, long b, int thr){
	long i;
	float val;
	float min_val = (float) thr / (float) P;
	float max_val = (float) (thr + 1) / (float) P; 
	long offset = (long) (thr * ((float) n / (float) P));
	long ind;
	for (i = 0; i < n; i++) {
		ind = (i + offset) % n;
		val = arr[ind];
		
		if ( min_val <= val && val < max_val){ 
			add_to_bucket(buckets, bucket_ind, val, b);
		}	
	} 
}

void sort_buckets(float** buckets, long* bucket_ind, long b) {
	long i;
	for (i = 0; i < b; i++){
		qsort(buckets[i], bucket_ind[i], sizeof(long), asc);
	}
}

// TODO make from and to
void merge_buckets(float* arr, float** buckets, long* bucket_ind, long cum_buck_ind,  long n, int from, int to){	
	long j;
	long bucket_num = 0;
	long i = from;
	while (i < to) {
		for (j = 0; j < bucket_ind[bucket_num]; j++) {
			arr[i] = buckets[bucket_num][j];
			i++;;
		}
		bucket_num++;
	}	
}

long* compute_cummulative_sum(long* arr, long n){
	long* cum_arr = init_ints(n);
	long i;
	cum_arr[0] = arr[0];
	for (i = 1; i < n; i++){
		cum_arr[i] = cum_arr[i-1] + arr[i];
	}		
	return cum_arr;
}
 
void print_int_arr(long* arr, long n) {
	long i;
	for (i = 0; i < n; i++) {
		printf("%ld, ", arr[i]);
	}
	printf("\n");
}


void print_float_arr(float* arr, long n){
	long i;
	for (i = 0; i < n; i++) {
		printf("%4f, ", arr[i]);
	} 
	printf("\n");
}

int main() {
	omp_set_num_threads(P);

	float* arr = init_array(N);
	float** buckets = init_2d_array(B, N);
	long* bucket_ind = init_ints(B);
	
	int thr;
	thr = 0;  //TODO change it later
	long long size_of_arr = N * sizeof(float);

	double t1 = omp_get_wtime();
	//#pragma omp parallel private (thr)
	//{
	thr = omp_get_thread_num();
	fill_array_rand(arr, N, thr);
	distribute_to_buckets(arr, buckets, bucket_ind, N, B, thr);
	sort_buckets(buckets, bucket_ind, B);  
	//}
	long* cum_buck_ind = compute_cummulative_sum(bucket_ind, B);
	// start second section
	merge_buckets(arr, buckets, bucket_ind, N);
	// sen second section
	double t2 = omp_get_wtime();
 	printf("%f,%lld\n", t2 - t1, size_of_arr);

	free(arr);
	free(bucket_ind);
	free(cum_buck_ind);
	free_2d_array(buckets, N);
	return 0;
}

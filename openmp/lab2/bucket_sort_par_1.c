#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#ifndef N
#define N 150000
#endif /*N*/

#ifndef B
#define B 500
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
	int* result = init_int_array(n);
	
	int i;
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

void fill_array_rand(float* arr, int n, int tid) {
	int i;
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

void add_to_bucket(float** buckets, int* bucket_ind, float val, int b){	
	//printf("%d\n", __LINE__);
        int number_of_bucket = (int) b*val;
	//printf("%d\n", __LINE__);
	int number_of_elements_in_bucket = bucket_ind[number_of_bucket];
	if (number_of_elements_in_bucket > N){
		printf("WTF: %d\n", number_of_elements_in_bucket);
	}  //TODO sprawdzic, co jest nie tak
	//printf("%d\n", __LINE__);
//	printf("%d %d\n", number_of_bucket, number_of_elements_in_bucket);
	buckets[number_of_bucket][number_of_elements_in_bucket] = val;	
//	printf("%d\n", __LINE__);
	bucket_ind[number_of_bucket]++;
	printf("%d\n", bucket_ind[number_of_bucket]);
//	printf("%d\n", __LINE__);
}

void distribute_to_buckets(float* arr, float** buckets, int* bucket_ind, int n, int b, int thr){
	int i;
	float val;
	float min_val = (float) thr / (float) P;
	float max_val = (float) (thr + 1) / (float) P; 
	int offset = (int) (thr * ((float) n / (float) P));
	int ind;
//	printf("%f %f %d\n", min_val, max_val, offset);
	for (i = 0; i < n; i++) {
		ind = (i + offset) % n;
		val = arr[ind];
		
//		printf("%d %f\n", ind, val);
		if ( min_val <= val && val < max_val){ 
			add_to_bucket(buckets, bucket_ind, val, b);
		}	
	} 
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
	
	int thr;
	thr = 0;  //TODO change it later
	long long size_of_arr = N * sizeof(float);

	double t1 = omp_get_wtime();
	//#pragma omp parallel private (thr)
	//{
	printf("%d\n", __LINE__);
	thr = omp_get_thread_num();
	printf("%d\n", __LINE__);
	fill_array_rand(arr, N, thr);
	printf("%d\n", __LINE__);
	distribute_to_buckets(arr, buckets, bucket_ind, N, B, thr);
	printf("%d\n", __LINE__);
	sort_buckets(buckets, bucket_ind, B);  
	//}
	// end first section
	int* cum_buck_ind = compute_cummulative_sum(bucket_ind, B);
//	print_int_arr(bucket_ind, B);
	// start second section
	merge_buckets(arr, buckets, bucket_ind, N);
	// sen second section
	double t2 = omp_get_wtime();
//	print_float_arr(arr, N);
 	 printf("%f,%lld\n", t2 - t1, size_of_arr);
	
	free(arr);
	free(bucket_ind);
	free(cum_buck_ind);
	free_2d_array(buckets, B);
	return 0;
}

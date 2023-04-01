#include <omp.h>
#include <stdio.h>

int main(){
	omp_set_num_threads(2);
	#pragma omp parallel
	printf("Hello world. Threads: %d\n", omp_get_thread_num());

	return 0;
}

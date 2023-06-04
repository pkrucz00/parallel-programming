#!/bin/bash


echo "TIME,ARR_SIZE,SCHEDULE_TYPE,CHUNK_SIZE,TEST_CASE_NUM"
for N in 1000000 10000000 100000000
do
	for schedule_type in {0..2}
	do 
		for chunk_size in -1 1 10 100
		do
		
			for test_case_num in {1..20}
			do
				gcc -Wall bucket_sort.c -o bucket_sort -fopenmp -DN=$N -DSCHEDULE_TYPE=$schedule_type -DCHUNK_SIZE=$chunk_size -DTEST_CASE_NUM=$test_case_num 
				./bucket_sort
			done
		done
	done
done

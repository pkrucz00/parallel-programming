#!/bin/bash


echo "TIME,N,SCHEDULE_TYPE,TEST_CASE_NUM"
for N in 1000000 10000000 100000000
do
#	for schedule_type in {1..12}
#	do 
#		for test_case_num in {1..20}
#		do
			gcc -Wall bucket_sort.c -o bucket_sort -fopenmp -DN=$N
			./bucket_sort
#		done
#	done
done

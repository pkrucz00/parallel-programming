#!/bin/bash

function mpi_run() {
    mpicc -o "$1.o" "$1.c" -lm "$3" "$4" 
    mpiexec -machinefile ./allnodes -np "$2" "./$1.o"
    rm "$1.o"
}

echo "PI,TIME,PROC_NUM,N,SCALING,TEST_CASE_NUM"
for N in 1000000 100000000 10000000000
do
	for proc_num in {1..12}
	do 
		for test_case_num in {1..15}
		do
			mpi_run monte $proc_num -DN=$N -DTEST_CASE_NUM=$test_case_num
			mpi_run monte_weak $proc_num -DN=$N -DTEST_CASE_NUM=$test_case_num
		done
	done
done

#!/bin/bash -l
#SBATCH --nodes 1
#SBATCH --ntasks 12
#SBATCH --time=01:00:00
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr23-cpu 

module add .plgrid plgrid/tools/openmpi

function mpi_run() {
    mpicc -o "$1.o" "$1.c" -lm "$3" "$4" 
    mpiexec -np "$2" "./$1.o"
    rm "$1.o"
}

echo "PI,TIME,PROC_NUM,N,SCALING,TEST_CASE_NUM"
for N in 1410065408
do
	for proc_num in 1
	do 
		for test_case_num in {1..10}
		do
			mpi_run monte $proc_num -DN=$N -DTEST_CASE_NUM=$test_case_num
		done
	done
done

#!/bin/bash -l
#SBATCH --nodes 1
#SBATCH --ntasks 12
#SBATCH --time=01:00:00
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr23-cpu 

module add .plgrid plgrid/tools/openmpi
mpicc -o how_fast.o monte_weak.c -DN=1410065408 -DTEST_CASE_NUM=2137
mpiexec -np 12 ./how_fast.o
rm how_fast.o

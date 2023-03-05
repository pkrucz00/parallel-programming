# parallel-programming

Course of Methods of Parallel Programming at AGH University of Science and Technology 2023

## How to compile
To compile `hello_world.c` programm simply type

``` bash
$ mpicc -o hello_world_c hello_world.c
$ mpiexec -machinefile ./allnodes -np [number of processors] ./hello_world_c 
```

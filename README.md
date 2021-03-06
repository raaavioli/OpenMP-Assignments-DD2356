# OpenMP-Assignments-DD2356

Programs assume your system supports OpenMP.

An important notice is that the program should be compiled with *CC*/*cc* and not *g++*/*gcc* if ran on KTH's supercomputer Beskow.

Questions and answers found on in [Google Docs](https://docs.google.com/document/d/1FMxgF0iNZ_2GWpm6JD9HD5PVGMx_ttCodM93w1tB8Ac/edit?usp=sharing).

## Exercise 1 - HelloOpenMP
### Build
```
g++ HelloOpenMP.cpp -o HelloOpenMP.out -fopenmp
```
### Run 
```
./HelloOpenMP.out <num_threads> 
```

## Exercise 2 - STREAM
Exercise 2 related to running STREAM benchmark and tracking bandwidth speedup. No code was developed.

## Exercise 3 - Parallel Sum
Exercise 3 relates benchmarking addition and comparing serial execution to different implementations of parallel summation.

### Build
```
g++ sum.cpp -fopenmp -o sum.out
```
### Run
#### Serial execution
```
./sum.out
```

#### Parallel execution
```
./sum.out <critical|omp|local|opt_local> <num_threads>
```

**critical**: Implementation using *omp critical*

**omp**: Implementation using *omp reduction*

**local**: Implementation calculating a local sum for each thread, and adding the results.

**opt\_local**: Same as local, except padding the thread local sums to avoid false sharing.

## Exercise 4 - DFTW
Exercise 4 relates to optimizing a serial DFT implementation using OpenMP. 

### Build
```
g++ DFTW_1.c -fopenmp -O2 -o dftw.out
```

### Run
```
./dftw
```

#### Number of threads
Change number of threads by setting the environment variable **OMP_NUM_THREADS** in your system.
```
export OMP_NUM_THREADS=<num_threads>
```

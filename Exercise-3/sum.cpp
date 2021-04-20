#include <stdlib.h>
#include <iostream>
#include <cstring>

#include <omp.h>

void print_help();
void generate_random(double *input, size_t size);

double serial_sum(double *x, size_t size) {
  double sum_val = 0.0;
  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }
  return sum_val;
}

double omp_sum(double *x, size_t size) {
  double sum = 0.0;
#pragma omp parallel reduction(+ : sum)
  {
  #pragma omp for schedule(static)
    for(int i = 0; i < size; i++) {
       sum += x[i];
    }
  }
  return sum;
}

double omp_critical_sum(double *x, size_t size) {
  double sum = 0.0;
  #pragma omp parallel
  {
    #pragma omp for schedule(static)
    for(int i = 0; i < size; i++) {
      #pragma omp critical
      {
        sum += x[i];
      }
    }
  }
  return sum;
}

double omp_local_sum(double *x, size_t size, uint32_t num_threads) {
  double sum = 0.0;
  double local_sum[num_threads];
  #pragma omp parallel
  {
    int id = omp_get_thread_num();
    local_sum[id] = 0.0;
    #pragma omp for schedule(static)
    for(int i = 0; i < size; i++) {
      local_sum[id] += x[i];
    }
  }

  for (int i = 0; i < num_threads; i++)
    sum += local_sum[i];
  return sum;
}

double opt_local_sum(double *x, size_t size, uint32_t num_threads) {
  // Padding container to avoid false sharing.
  // Memory inefficient but performance effective
  struct Container { double sum; char pad[128]; };
  Container local_sum[num_threads];
 
  double sum = 0.0;

  #pragma omp parallel
  {
    int id = omp_get_thread_num();
    local_sum[id].sum = 0.0;
    #pragma omp for schedule(static)
    for(int i = 0; i < size; i++) {
      local_sum[id].sum += x[i];
    }
  }

  for (int i = 0; i < num_threads; i++)
    sum += local_sum[i].sum;
  return sum;
}

int main(int argc, char* argv[]) {	
  int size = 1e+7;
  double *input = new double[size];
  generate_random(input, size);

  if (argc == 1 || argc == 3 && isdigit(argv[2][0])) { 
    double total_time = 0.0;
    double run_sum = 0.0;
    double warmup_sum = 0.0;
    const char* benchmark_name = argc == 1 ? "serial" : argv[1];
    if (argc == 1) {
      /** Serial */
      std::cout << "Running benchmark: " << benchmark_name << std::endl; 
      // Warm up
      warmup_sum = serial_sum(input, size);
      // Benchmark
      double start = omp_get_wtime();
      run_sum = serial_sum(input, size);
      total_time = omp_get_wtime() - start;
    } else {
      /** Parallel */
      uint32_t num_threads = atoi(argv[2]);
      omp_set_num_threads(num_threads);
      if (strncmp(benchmark_name, "critical", 8) == 0) {
	/** OpenMP Critical sum */
        std::cout << "Running benchmark: " << benchmark_name << " on " << num_threads << " threads." << std::endl; 
        warmup_sum = omp_critical_sum(input, size);
        double start = omp_get_wtime();
        run_sum = omp_critical_sum(input, size);
        total_time = omp_get_wtime() - start;
      } else if (strncmp(benchmark_name, "omp", 3) == 0) {
	/** OpenMP sum (Reduction) */
        std::cout << "Running benchmark: " << benchmark_name << " on " << num_threads << " threads." << std::endl; 
        warmup_sum = omp_sum(input, size);
        double start = omp_get_wtime();
        run_sum = omp_sum(input, size);
        total_time = omp_get_wtime() - start;
      } else if (strncmp(benchmark_name, "local", 5) == 0) {
	/** OpenMP Local sum */
        std::cout << "Running benchmark: " << benchmark_name << " on " << num_threads << " threads." << std::endl; 
        warmup_sum = omp_local_sum(input, size, num_threads);
        double start = omp_get_wtime();
        run_sum = omp_local_sum(input, size, num_threads);
        total_time = omp_get_wtime() - start; 
      } else if (strncmp(benchmark_name, "opt_local", 9) == 0) {
        /** OpenMP Local Optimized sum */
        std::cout << "Running benchmark: " << benchmark_name << " on " << num_threads << " threads." << std::endl; 
        warmup_sum = opt_local_sum(input, size, num_threads);
        double start = omp_get_wtime();
        run_sum = opt_local_sum(input, size, num_threads);
        total_time = omp_get_wtime() - start;
      } else {
	std::cout << "Error: No benchmark named: " << benchmark_name << std::endl;
      }
    }

    if (total_time) {
      std::cout << "Use results to avoid compiler optimizations. warmup: " << warmup_sum << " run: " << run_sum << std::endl;
      std::cout << benchmark_name <<" execution time (s): " << total_time << "" << std::endl;
    }
  } else {
    print_help();
  }

  delete[] input;
}

void generate_random(double *input, size_t size) {
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}

void print_help() {
  std::cout << "Program can be run in serial or in parallel." << std::endl;
  std::cout << "Sum serial: ./<binary>" << std::endl;
  std::cout << "Sum parallel: ./<binary> <critical|omp|local|opt_local> <num_threads>" << std::endl << std::endl;
  std::cout << "Serial execution differs from parallel execution on one thread." << std::endl;
  std::cout << "./<binary> is not equal to ./<binary> omp 1." << std::endl;
}

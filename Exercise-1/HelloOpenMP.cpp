#include "omp.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
	int num_threads = 4;
	if (argc == 2) {
		num_threads = atoi(argv[1]);
	}
	omp_set_num_threads(num_threads);
#pragma omp parallel
	{
		int id = omp_get_thread_num();
		printf("Hello World from Thread %d!\n", id);
	}
}

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"

int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);
    double end, start = MPI_Wtime();
    int rank, size, i;
    long steps = 10000000;
    double x, sum=0.0;

    // set the number of steps
    if (argc > 1) {
        steps = atoi(argv[1]);
    }

    // calculate how big a step is
    double step = 1/(double)steps;

    // get our rank and how many procs there are
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // figure out how many we need to calculate locally
    // then calculate our sum
    int steps_per_proc = steps / size;
    for (i = 0; i < steps_per_proc; i++) {
        x = ((i+rank*steps_per_proc)+.5)*step;
        sum += 4.0/(1.0+x*x);
    }

    // get the total sum of all procs
    double total_sum = 0.0;
    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // compute pi
    double pi = total_sum * step;

    // we are done print results
    end = MPI_Wtime();
    if (rank == 0) {
        printf("execution time: %.8f seconds\n", end - start);
        printf("pi: %.8f\n", pi);
    }

    MPI_Finalize();
    return 0;
}


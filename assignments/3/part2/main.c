#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mpi.h"

double get_rand_between_0_and_1()
{
    return (double)rand() / (double)RAND_MAX;
}

double** generate_a(int n)
{
    double** ret =  malloc(sizeof(double*) * n);
    for (int i = 0; i < n; i++) {
        ret[i] = malloc(sizeof(double) * n);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ret[i][j] = get_rand_between_0_and_1();
        }
    }

    return ret;
}

double* generate_x(int n)
{
    double* ret = malloc(n*sizeof(double));

    for (int i = 0; i < n; i++) {
        ret[i] = get_rand_between_0_and_1();
    }

    return ret;
}

int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);
    srand(435);
    int rank, size, i;
    int n = 128;
    double*  x = generate_x(n);
    double** a = generate_a(n);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // so even though all the processes have the entirety of x
    // I am going to treat it as if they only have n/p of them
    // to mimic the algo from lecture as much as possible
    int vals_per_proc = n / size;
    double* local_xs = malloc(sizeof(double)*vals_per_proc);
    double* all_xs = malloc(sizeof(double)*n);
    double* local_ys = malloc(sizeof(double)*vals_per_proc);
    double* all_ys = malloc(sizeof(double)*n);

    double start = MPI_Wtime();

    for (int i = vals_per_proc*rank; i < vals_per_proc*rank+vals_per_proc; i++) {
        local_xs[i-vals_per_proc*rank] = x[i];
    }

    int ret = MPI_Allgather(local_xs, vals_per_proc, MPI_DOUBLE, all_xs, vals_per_proc, MPI_DOUBLE, MPI_COMM_WORLD);


    for (int i = 0; i < vals_per_proc; i++) {
        for (int j = 0; j < n; j++) {
            local_ys[i] += a[i][j] * all_xs[j];
        }
    }

    MPI_Allgather(local_ys, vals_per_proc, MPI_DOUBLE, all_ys, vals_per_proc, MPI_DOUBLE, MPI_COMM_WORLD);

    double end = MPI_Wtime();
    // every process now has y
    printf("rank: %d, time: %lf seconds\n", rank, end-start);

    MPI_Finalize();
    return 0;
}


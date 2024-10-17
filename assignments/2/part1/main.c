#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mpi.h"


int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);
    int rank, size, i;
    double *local_as = NULL, *a = NULL;
    double x = .5;
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int vals_per_rank = 64/size;
    printf("rank %d: vals_per_rank = %d\n", rank, vals_per_rank);
    local_as = calloc(vals_per_rank, sizeof(double));

    if (rank == 0) {
        // make the random values
        a = calloc(64, sizeof(double));

        srand(0);
        for (i = 0; i < 64; i++) {
            a[i] = (double)rand()/RAND_MAX;
            printf("rank %d: a[%d] = %.8f\n", rank, i, a[i]);
        }
    }

    // scatter the a's to all processes
    MPI_Scatter(a, vals_per_rank, MPI_DOUBLE, local_as, vals_per_rank, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // calculate x's
    double *xs = calloc(vals_per_rank, sizeof(double));
    double *xs2 = calloc(vals_per_rank, sizeof(double));
    double tmp = x;
    for (i = 0; i < vals_per_rank; i++, tmp *= x) {
        xs[i] = tmp;
        printf("rank %d: xs[%d] = %.8f\n", rank, i, xs[i]);
    }
    
    double max_x = xs[vals_per_rank-1];
    double x_to_mult = 0;

    // MPI_Exscan to get the result of the multiplication of the previous highest values.
    // aka the value of x that will become the basis for ours
    MPI_Exscan(&max_x, &x_to_mult, 1, MPI_DOUBLE, MPI_PROD, MPI_COMM_WORLD);

    printf("rank %d: x_to_mult = %f\n", rank, x_to_mult);

    // multiply to make the values correct
    if (rank != 0) {
        for (i = 0; i < vals_per_rank; i++) {
            xs[i] = xs[i] * x_to_mult;
        }
    }


    // multiply by proper random value and sum
    double local_sum = 0;
    for (i = 0; i < vals_per_rank; i++) {
        local_sum += local_as[i] * xs[i];
    }

    printf("rank %d: local_sum = %.8f\n", rank, local_sum);

    // MPI_Reduce
    double total_sum = 0;

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("total = %.8f\n", total_sum);
    }

    if (xs) free(xs);
    if (local_as) (local_as);
    if (a) free(a);
    MPI_Finalize();
    return 0;
}


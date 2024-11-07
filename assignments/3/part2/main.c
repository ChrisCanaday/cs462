#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
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

double*** gen_submatrices(int n_per_proc, int total_procs, double** matrix)
{
    int p = (int) sqrt(total_procs);
    double*** sub_matrices = malloc(sizeof(double**)*total_procs);
    for (int i = 0; i < total_procs; i++) {
        sub_matrices[i] = malloc(sizeof(double*)*n_per_proc);
        for (int j = 0; j < n_per_proc; j++) {
            sub_matrices[i][j] = malloc(sizeof(double)*n_per_proc);
            for (int k = 0; k < n_per_proc; k++) {
                sub_matrices[i][j][k] = matrix[j+(i/p)*n_per_proc][k+n_per_proc*(i%p)];
            }
        }
    }

    return sub_matrices;
}

double* oneDify(int n_per_proc, int total_procs, double*** sub_matrices)
{
    double* ret = malloc(sizeof(double)*total_procs*n_per_proc*n_per_proc);

    for (int i = 0; i < total_procs; i++) {
        for (int j = 0; j < n_per_proc; j++) {
            for (int k = 0; k < n_per_proc; k++) {
                ret[i*n_per_proc*n_per_proc + j*n_per_proc + k] = sub_matrices[i][j][k];
            }
        }
    }

    return ret;
}

void print_vector(int length, double* vector) {
    printf( "[");
    for(int i = 0; i < length; i++) {
        if (i == 0) {
            printf("%lf", vector[i]);
        }else {
            printf(" %lf", vector[i]);
        }
    }
    printf("]\n\n");
}

int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);
    srand(435);
    int rank, size;
    int n = 128;
    double*  x = generate_x(n);
    double** a = generate_a(n);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    double start = MPI_Wtime();
    int p = (int) sqrt(size);

    // so even though all the processes have all the stuff I am going to pretend
    // that rank 0 is the only one with the stuff to follow the algo
    // as much as possible
    int vals_per_proc = n / p;
    double* local_xs = malloc(sizeof(double)*vals_per_proc);
    double* local_ys = calloc(vals_per_proc, sizeof(double));
    double* local_as = malloc(vals_per_proc*vals_per_proc*sizeof(double));
    double*** submatrices = NULL;
    double* oneD = NULL;

    // create the submatrices of a
    if (rank == 0) {
        submatrices = gen_submatrices(vals_per_proc, size, a);
        oneD = oneDify(vals_per_proc, size, submatrices);
    }

    // send out all the submatrices of a
    MPI_Scatter(oneD, vals_per_proc*vals_per_proc, MPI_DOUBLE, local_as, vals_per_proc*vals_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // free the small a stuff
    if (rank == 0) {
        free(oneD);
        
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < vals_per_proc; j++) {
                free(submatrices[i][j]);
            }
            free(submatrices[i]);
        }
        free(submatrices);
    }

    // colors for each proc
    // this seperates them into rows, cols, and diag
    // diag in the left diag starting at 0 and ending at size-1
    int row_color = rank / p;
    int col_color = rank % p;
    int diag_color = (rank/p == rank%p) ? 0 : 1;

    // create the communicators
    MPI_Comm row_comm, col_comm, diag_comm;
    MPI_Comm_split(MPI_COMM_WORLD, row_color, rank, &row_comm);
    MPI_Comm_split(MPI_COMM_WORLD, col_color, rank, &col_comm);
    MPI_Comm_split(MPI_COMM_WORLD, diag_color, rank, &diag_comm);

    // grab your row rank
    // (don't need col or diag rank for our uses)
    int row_rank;
    MPI_Comm_rank(row_comm, &row_rank);

    // diagonal
    if (rank/p == rank%p) {
        // move the portions of x to the correct diagonal proc
        MPI_Scatter(x, vals_per_proc, MPI_DOUBLE, local_xs, vals_per_proc, MPI_DOUBLE, 0, diag_comm);
    }

    // broadcast the x to the rest of the column
    MPI_Bcast(local_xs, vals_per_proc, MPI_DOUBLE, rank%p, col_comm);

    // all procs should have their local x now :D
    // do the local calculation to get this procs section of the y vector
    for (int i = 0; i < vals_per_proc; i++) {
        for (int j = 0; j < vals_per_proc; j++) {
            local_ys[i] += local_as[i*vals_per_proc+j] * local_xs[j];
        }
    }

    double* row_ys = NULL;

    // only last proc in row needs this array
    if (row_rank == p-1) {
        row_ys = malloc(sizeof(double)*vals_per_proc);
    }

    // add up everythin in your row. result is in the farthest right proc in the row
    MPI_Reduce(local_ys, row_ys, vals_per_proc, MPI_DOUBLE, MPI_SUM, p-1, row_comm);

    // concat all columns together
    double* final_y = malloc(sizeof(double)*n);
    if (p-1 == rank%p) {
        MPI_Allgather(row_ys, vals_per_proc, MPI_DOUBLE, final_y, vals_per_proc, MPI_DOUBLE, col_comm);
    }

    // broadcast the answer to all the procs (bottom right proc will be the sender)
    MPI_Bcast(final_y, n, MPI_DOUBLE, size-1, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0) {
        printf("time to compute y = %.10lf seconds\n", end - start);
        print_vector(n, final_y);
    }

    // clean up memory
    free(final_y);
    free(local_as);
    free(local_ys);
    free(local_xs);
    free(x);
    for(int i = 0; i < n; i++) {
        free(a[i]);
    }
    free(a);

    MPI_Finalize();
    return 0;
}

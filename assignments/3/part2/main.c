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

int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);
    srand(435);
    int rank, size;
    int n = 128;
    double*  x = generate_x(n);
    double** a = generate_a(n);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int p = (int) sqrt(size);

    // so even though all the processes have the entirety of x
    // I am going to treat it as if they only have n/p of them
    // to mimic the algo from lecture as much as possible
    int valsx_per_proc = n / p;
    //int valsx_per_proc = n / size;
    double* local_xs = malloc(sizeof(double)*valsx_per_proc);
    double* all_xs = malloc(sizeof(double)*n);
    double* local_ys = calloc(valsx_per_proc, sizeof(double));
    double* local_as = malloc(valsx_per_proc*sizeof(double));
    double* all_ys = malloc(sizeof(double)*n);
    int** groups = malloc(sizeof(int*)*p);
    int** rowgroups = malloc(sizeof(int*)*p);
    int* diag_ranks = malloc(sizeof(int)*p);
    double*** submatrices = NULL;
    double* oneD = NULL;

    if (rank == 0) {
        submatrices = gen_submatrices(valsx_per_proc, size, a);
        oneD = oneDify(valsx_per_proc, size, submatrices);
    }

    // send out all the local a's
    MPI_Scatter(oneD, valsx_per_proc, MPI_DOUBLE, local_as, valsx_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        free(oneD);
        
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < valsx_per_proc; j++) {
                free(submatrices[i][j]);
            }

            free(submatrices[i]);
        }
        free(submatrices);
    }

    // make the column groups
    for (int i = 0; i < p; i++) {
        int* list = malloc(sizeof(int)*p);
        int* rowlist = malloc(sizeof(int)*p);

        for (int j = 0; j < p; j++) {
            list[j] = j*p + i;
            rowlist[j] = i*p + j;
        }

        groups[i] = list;
        rowgroups[i] = rowlist;

        if (rank/p == rank%p) {
            diag_ranks[i] = rank;
        }
    }

    // print groups and rowgroups
    // for (int i = 0; i < p; i++) {
    //     printf("group %d: col:(", i);
    //     for (int j = 0; j < p; j++) {
    //         printf("%d,", groups[i][j]);
    //     }
    //     printf(") row:(");
    //     for (int j = 0; j < p; j++) {
    //         printf("%d,", rowgroups[i][j]);
    //     }
    //     printf(")\n");
    // }

    //printf("before world %d\n", rank);
//     MPI_Group world;
//     MPI_Comm_group(MPI_COMM_WORLD, &world);
//     //printf("before col %d\n", rank);
//     MPI_Group column_group;
//     MPI_Group_incl(world, p, groups[rank%p], &column_group);
//    // printf("before col comm %d\n", rank);
//     MPI_Comm column_comm = MPI_COMM_NULL;
//     printf("rank: %d, coltag: %d\n", rank, 1 + rank%p);
//     MPI_Comm_create_group(MPI_COMM_WORLD, column_group, 1 + rank%p, &column_comm);
//     //printf("before row %d\n", rank);
//     MPI_Group row_group;
//     MPI_Group_incl(world, p, groups[rank/p], &row_group);
//     //printf("before row comm %d\n", rank);
//     MPI_Comm row_comm = MPI_COMM_NULL;
//     printf("rank: %d, rowtag: %d\n", rank, 1 + p + rank/p);
//     MPI_Comm_create_group(MPI_COMM_WORLD, row_group, 1 + p + rank/p, &row_comm);
    //printf("after coms %d\n", rank);

    int row_color = rank / p;
    int col_color = rank % p;

    MPI_Comm row_comm, col_comm;

    MPI_Comm_split(MPI_COMM_WORLD, row_color, rank, &row_comm);
    MPI_Comm_split(MPI_COMM_WORLD, col_color, rank, &col_comm);

    int row_rank, col_rank;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_rank(col_comm, &col_rank);

    printf("rank: %d, row_rank: %d, col_rank: %d\n", rank, row_rank, col_rank);



    // for (int i = valsx_per_proc*rank; i < valsx_per_proc*rank+valsx_per_proc; i++) {
    //     local_xs[i-valsx_per_proc*rank] = x[i];
    // }

    // diagonal
    if (rank/p == rank%p) {
        MPI_Group world;
        MPI_Comm_group(MPI_COMM_WORLD, &world);
        MPI_Group diag_group;
        MPI_Group_incl(world, p, diag_ranks, &diag_group);
        MPI_Comm diag_comm = MPI_COMM_NULL;
        printf("hi\n");
        MPI_Comm_create_group(MPI_COMM_WORLD, diag_group, 3, &diag_comm);
        printf("bye\n");

        // move the portions of x to the correct diagonal proc
        printf("before scatter %d\n", rank);
        MPI_Scatter(x, valsx_per_proc, MPI_DOUBLE, local_xs, valsx_per_proc, MPI_DOUBLE, 0, diag_comm);
        printf("done with scatter %d\n", rank);
    }

    // cascade the x to the rest of the column
    MPI_Bcast(local_xs, valsx_per_proc, MPI_DOUBLE, rank%p, col_comm);

    printf("done with bcast: %d\n", rank);


    // all procs should have their local x now :D
    for (int i = 0; i < valsx_per_proc; i++) {
        for (int j = 0; j < valsx_per_proc; j++) {
            local_ys[i] += local_as[i*valsx_per_proc+j] * local_xs[j];
        }
    }

    double* row_ys = NULL;

    // only last proc in row needs this array
    if (rank == rowgroups[rank/p][p-1]) {
        row_ys = malloc(sizeof(double)*valsx_per_proc);
    }
    int rank2;
    MPI_Comm_rank(row_comm, &rank2);

    printf("before reduce rank2: %d rank: %d rank/p: %d p: %d reduce-target: %d\n", rank2, rank, rank/p, p, rowgroups[rank/p][p-1]);
    MPI_Barrier(MPI_COMM_WORLD);
    // add the vectors of sums with other procs in your row
    MPI_Reduce(local_ys, row_ys, valsx_per_proc, MPI_DOUBLE, MPI_SUM, rowgroups[rank/p][p-1], row_comm);

    // concat all columns together
    double* final_y = malloc(sizeof(double)*n);
    if (p-1 == rank%p) {
        MPI_Allgather(row_ys, valsx_per_proc, MPI_DOUBLE, final_y, valsx_per_proc, MPI_DOUBLE, col_comm);
    }

    // broadcast the answer to all other procs
    MPI_Bcast(final_y, n, MPI_DOUBLE, rank/p*p+p-1, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("final_y =\n[\n");
        for(int i = 0; i < p; i++) {
            for (int j = 0; j < valsx_per_proc; j++) {
                printf(" %lf", final_y[i*valsx_per_proc + j]);
            }
            printf("\n");
        }
    }



    // print answer on proc 0

    // for (int i = 0; i < valsx_per_proc; i++) {
    //     for (int j = 0; j < n; j++) {
    //         local_ys[i] += local_as[i][j] * all_xs[j];
    //     }
    // }



    // double start = MPI_Wtime();


    // int ret = MPI_Allgather(local_xs, valsx_per_proc, MPI_DOUBLE, all_xs, valsx_per_proc, MPI_DOUBLE, MPI_COMM_WORLD);


    // for (int i = 0; i < valsx_per_proc; i++) {
    //     for (int j = 0; j < n; j++) {
    //         local_ys[i] += a[i][j] * all_xs[j];
    //     }
    // }

    // MPI_Allgather(local_ys, valsx_per_proc, MPI_DOUBLE, all_ys, valsx_per_proc, MPI_DOUBLE, MPI_COMM_WORLD);

    // double end = MPI_Wtime();
    // every process now has y
    //printf("rank: %d, time: %lf seconds\n", rank, end-start);

    MPI_Finalize();
    return 0;
}


//  Christopher Canaday
//  COSC 462 Assignment 1
// Collective Communication

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char **argv ) {
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int unique_int = rank + 1;
    int *the_ints = NULL;
    int sum = 0;
    int product = 0;

    if (rank == 0) {
        the_ints = malloc(size*sizeof(int));
    }

    MPI_Gather(&unique_int, 1, MPI_INT, the_ints, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("MPI_Gather | Rank 0 has integers:");
        for(int i = 0; i < size; i++) {
            printf(" %d", the_ints[i]);
        }
        printf("\n");

        free(the_ints);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    the_ints = malloc(size*sizeof(int));
    MPI_Allgather(&unique_int, 1, MPI_INT, the_ints, 1, MPI_INT, MPI_COMM_WORLD);

    printf("MPI_Allgather |Rank %d has integers:", rank);
    for(int i = 0; i < size; i++) {
        printf(" %d", the_ints[i]);
    }
    printf("\n");

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Reduce(&unique_int, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("MPI_Reduce | Rank 0 sum: %d\n", sum);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Reduce(&unique_int, &product, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("MPI_Reduce | Rank 0 prod: %d\n", product);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allreduce(&unique_int, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    printf("MPI_Allreduce | Rank %d sum: %d\n", rank, sum);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allreduce(&unique_int, &product, 1, MPI_INT, MPI_PROD, MPI_COMM_WORLD);
    printf("MPI_Allreduce | Rank %d prod: %d\n", rank, product);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
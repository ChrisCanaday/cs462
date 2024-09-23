//  Christopher Canaday
//  COSC 462 Assignment 1
// Point-to-Point Communication Part 2

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char **argv ) {
    int rank, size;
    double start, end;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    


    MPI_Finalize();
    return 0;
}
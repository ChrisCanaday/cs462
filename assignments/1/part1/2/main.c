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

    if (rank == 0 || rank == 95) {
        // send messages of size 2 byte -> 1 GB
        for (int i = 1; i <= 30; i++) {
            size_t size = 1 << i;
            char* rand_text = (char*) malloc(size);

            if (rank == 0) {

                start = MPI_Wtime();
                MPI_Send(rand_text, size, MPI_CHAR, 95, 0, MPI_COMM_WORLD);
                end = MPI_Wtime();

                printf("Process %d took %lf seconds to send message of size %f to process %d\n", 0, end - start, num_bytes, 95);
            }else if (rank == 95) {
                MPI_Recv(rand_text, size, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            }

            free(rand_text);
        }
    }

    MPI_Finalize();
    return 0;
}
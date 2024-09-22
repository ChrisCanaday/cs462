//  Christopher Canaday
//  COSC 462 Assignment 1
// Point-to-Point Communication Part 2

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
//#include <math.h>
#include "mpi.h"

int main(int argc, char **argv ) {
    int rank, size;
    double start, end;
    double num_bytes;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0 || rank == 95) {
        // send messages of size 2 byte -> 1 GB
        for (int i = 1; i <= 30; i++) {
            num_bytes = 1 << i;
            size_t size = (size_t) num_bytes;
            printf("size: %lu || num_bytes %f\n", size, num_bytes);
            char* rand_text = (char*) malloc(size);

            if (rank == 0) {
                start = MPI_Wtime();
                MPI_Send(rand_text, size, MPI_CHAR, 95, 0, MPI_COMM_WORLD);
                end = MPI_Wtime();

                printf("Process %d took %lf seconds to send message of size %f to process %d\n", 0, end - start, num_bytes, 95);
            }else if (rank == 95) {
                start = MPI_Wtime();
                MPI_Recv(rand_text, size, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                end = MPI_Wtime();

                printf("Process %d took %lf seconds to receive message of size %f to process %d\n", 95, end - start, num_bytes, status.MPI_SOURCE);
            }

            free(rand_text);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
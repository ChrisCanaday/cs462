//  Christopher Canaday
//  COSC 462 Assignment 1
// Point-to-Point Communication Part 1

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "mpi.h"

int get_left_process(int rank, int size) {
    return (rank) ? (rank-1) % size : size-1;
}

int get_right_process(int rank, int size) {
    return (rank+1) % size;
}

void send_to_target(int target, int *to_send, int len, int rank) {
    int to_recv = 0;
    MPI_Status status;

    MPI_Send(to_send, len, MPI_INT, target, 0, MPI_COMM_WORLD);
    MPI_Recv(&to_recv, sizeof(to_recv), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    printf("Process %d received %d from process %d\n", rank, to_recv, status.MPI_SOURCE);
}

int main(int argc, char **argv ) {
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int to_send = rank;
    int target = 0;

    target = get_right_process(rank, size);
    send_to_target(target, &to_send, sizeof(to_send), rank);

    target = get_left_process(rank, size);
    send_to_target(target, &to_send, sizeof(to_send), rank);

    MPI_Finalize();
    return 0;
}
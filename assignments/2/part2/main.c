#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"

#define nsteps 10000000
void main_serial()
{
    double x, sum=0.0;
    double step = 1.0/nsteps;
    for (int i=0; i<nsteps; i++)
    {
        x = (i+0.5)*step;
        sum += 4.0/(1.0+x*x);
    }
    double pi = sum*step;
    printf("The value of pi = %.8f\n\t%.8f * %.8f\n",pi, sum, step);
}


int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);
    int rank, size, i;
    long steps = 10000000;
    double step = 1/(double)steps;
    double x, sum=0.0;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        main_serial();
    }

    int steps_per_proc = steps / size;

    for (i = 0; i < steps_per_proc; i++) {
        x = ((i+rank*steps_per_proc)+.5)*step;
        sum += 4.0/(1.0+x*x);
    }

    double total_sum = 0.0;
    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi = total_sum * step;
        printf("total_sum = %.8f\n\t %.8f * %.8f\n", pi, total_sum, step);
    }


    MPI_Finalize();
    return 0;
}


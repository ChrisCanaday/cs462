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
    printf("The value of pi = %f\n",pi);
}


int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);

    MPI_Finalize();
    return 0;
}


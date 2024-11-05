#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mpi.h"


int main(int argc, char **argv ) {
    MPI_Init(&argc, &argv);

    MPI_Finalize();
    return 0;
}


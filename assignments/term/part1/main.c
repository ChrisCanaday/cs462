#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

double** generate_matrix_of_1s(int n)
{
    double** ret =  malloc(sizeof(double*) * n);
    for (int i = 0; i < n; i++) {
        ret[i] = malloc(sizeof(double) * n);
    }

    for (int i = 0; i < n; i++) {
        memset(ret[i], 1, n*sizeof(double));
    }

    return ret;
}

void print_matrix(int height, int width, double** matrix)
{
    printf("[");
    for(int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i == 0 && j == 0) {
                printf("%lf", matrix[i][j]);
            }else {
                printf(" %lf", matrix[i][j]);
            }
        }
    }
    printf("]\n\n");
}

void print_vector(int length, double* vector) {
    printf("[");
    for(int i = 0; i < length; i++) {
        if (i == 0) {
            printf("%lf", vector[i]);
        }else {
            printf(" %lf", vector[i]);
        }
    }
    printf("]\n\n");
}


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    srand(435);

    int n = 128;

    double** A = generate_matrix_of_1s(n);
    double** B = generate_matrix_of_1s(n);

    print_matrix(n,n,A);
    print_matrix(n,n,B);

    MPI_Finalize();
    return 0;
}
#include <stdlib.h>
#include <stdio.h>
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

double* matrix_mult_by_vector(int n, double** a, double* x)
{
    double* y = calloc(n, sizeof(double));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            y[i] += a[i][j] *  x[j];
        }

    }

    return y;
}


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    srand(435);

    int n = 128;

    double *x = generate_x(n);
    double **a = generate_a(n);

    double start = MPI_Wtime();
    double* y = matrix_mult_by_vector(n, a, x);
    double end = MPI_Wtime();

    printf("time to compute y = %.10lf seconds\n", end-start);

    print_vector(n,y);

    free(y);
    free(x);
    for(int i = 0; i < n; i++) {
        free(a[i]);
    }

    free(a);

    MPI_Finalize();
    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "mpi.h"

int p = 1;
int P = 1;
int n = 128;

double** generate_matrix(int n, double val)
{
    double** ret =  malloc(sizeof(double*) * n);
    for (int i = 0; i < n; i++) {
        ret[i] = malloc(sizeof(double) * n);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ret[i][j] = val;
        }
    }

    return ret;
}

bool strmatch(char* first, char* second)
{
    return (strcmp(first, second) == 0);
}

void print_usage()
{
    printf("Usage:\n");
    printf("./a.out [options]\n");
    printf("   Options:\n");
    printf("      -P [] : Number of processors to use. (is p*p)\n");
    printf("      -p [] : Sqrt of P\n");
    printf("      -n [] : Height and width of the matrix\n");
}

bool parse_args(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (strmatch(arg, "-p")) {
            i += 1;
            p = atoi(argv[i]);
            P = p*p;
        }else if (strmatch(arg, "-P")) {
            i += 1;
            P = atoi(argv[i]);
            p = (int) sqrt(P);
        } else if (strmatch(arg, "-n")) {
            i += 1;
            n = atoi(argv[i]);
        } else {
            return false;
        }
    }

    return true;
}

double*** gen_submatrices(double** matrix)
{
    int n_per_proc = n / p;
    double*** sub_matrices = malloc(sizeof(double**)*P);
    for (int i = 0; i < P; i++) {
        sub_matrices[i] = malloc(sizeof(double*)*n_per_proc);
        for (int j = 0; j < n_per_proc; j++) {
            sub_matrices[i][j] = malloc(sizeof(double)*n_per_proc);
            for (int k = 0; k < n_per_proc; k++) {
                sub_matrices[i][j][k] = matrix[j+(i/p)*n_per_proc][k+n_per_proc*(i%p)];
            }
        }
    }

    return sub_matrices;
}

double* oneDify(double*** sub_matrices)
{
    int n_per_proc = n / p;
    double* ret = malloc(sizeof(double)*P*n_per_proc*n_per_proc);

    for (int i = 0; i < P; i++) {
        for (int j = 0; j < n_per_proc; j++) {
            for (int k = 0; k < n_per_proc; k++) {
                ret[i*n_per_proc*n_per_proc + j*n_per_proc + k] = sub_matrices[i][j][k];
            }
        }
    }

    return ret;
}

double* oneDify2(double** sub_matrices)
{
    int n_per_proc = n / p;
    double* ret = malloc(sizeof(double)*n_per_proc*n_per_proc);

    for (int i = 0; i < n_per_proc; i++) {
        for (int j = 0; j < n_per_proc; j++) {
                ret[i*n_per_proc + j] = sub_matrices[i][j];
        }
    }

    return ret;
}

double* matrix_mult(int n, double* A, double* B)
{
    double* C = calloc(n*n,sizeof(double));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                C[i*n + j] += A[i*n + k] * B[k*n + j];
            }
        }
    }

    return C;
}

void add_matrix_to_output(int n, double* A, double* output)
{
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            output[i*n+j] += A[i*n+j];
        }
    }
}

void swap_matrices(int n, double** A, double** B)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double tmp = A[i][j];
            A[i][j] = B[i][j];
            B[i][j] = tmp;
        }
    }
}

int main(int argc, char** argv)
{
    double** A = NULL;
    double** B = NULL;
    double*** sub_matricesA = NULL;
    double*** sub_matricesB = NULL;
    double* scatterable_subA = NULL; // for MPI_Scatter
    double* scatterable_subB = NULL; // for MPI_Scatter
    int rank, size;
    double start, end, comp_time = 0, comm_time = 0, total_time = 0;
    
    // bail early if args are garbage
    if (!parse_args(argc, argv)) {
        printf("invalid args\n");
        print_usage();
        return 0;
    }
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n_per_proc = n / p;
    int row = rank/p;
    int col = rank%p;
    // figure out your neighbors
    int up = (row-1 < 0) ? p*(p-1) + col: (row-1)*p + col;
    int down = (row+1 == p) ? 0 + col: (row+1)*p + col;
    int left = (col-1 < 0) ? (p-1)+row*p: col-1 + row*p;
    int right = (col+1 == p) ? 0 + row*p: col+1 + row*p;

    if (rank == 0) {
        A = generate_matrix(n, 1);
        B = generate_matrix(n, 1);
        sub_matricesA = gen_submatrices(A);
        sub_matricesB = gen_submatrices(B);

        // shift a's
        for (int i = 0; i < p; i++) {
            // how many shift rounds to do
            for (int j = 0; j < i; j++) {
                // swap all the stuff
                for (int k = 0; k < p-1; k++) {
                    swap_matrices(n_per_proc, sub_matricesA[i*p+k], sub_matricesA[i*p+k+1]);
                }
            }
        }

        // shift b's
        for (int i = 0; i < p; i++) {
            // how many shift rounds to do
            for (int j = 0; j < i; j++) {
                // swap all the stuff
                for (int k = 0; k < p-1; k++) {
                    swap_matrices(n_per_proc, sub_matricesB[k*p+i], sub_matricesB[(k+1)*p+i]);
                }
            }
        }

        // this is a bit wasteful but I did it the first way first and its staying like that until I wanna fix it
        // which I probably never will
        scatterable_subA = oneDify(sub_matricesA);
        scatterable_subB = oneDify(sub_matricesB);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double* local_subA = malloc(sizeof(double) * n_per_proc*n_per_proc);
    double* local_subB = malloc(sizeof(double) * n_per_proc*n_per_proc);

    start = MPI_Wtime();
    MPI_Scatter(scatterable_subA, n_per_proc*n_per_proc, MPI_DOUBLE, local_subA, n_per_proc*n_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(scatterable_subB, n_per_proc*n_per_proc, MPI_DOUBLE, local_subB, n_per_proc*n_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();

    comm_time += end - start;

    // its cannon time baby
    double* tmp = NULL;
    double* tmpA = malloc(sizeof(double) * n_per_proc*n_per_proc);
    double* tmpB = malloc(sizeof(double) * n_per_proc*n_per_proc);
    double** Corig = generate_matrix(n_per_proc, 0);
    double*  C = oneDify2(Corig);
    MPI_Request request;
    MPI_Status status;
    for (int i = 0; i < p; i++) {
        start = MPI_Wtime();
        tmp = matrix_mult(n_per_proc, local_subA, local_subB);
        add_matrix_to_output(n_per_proc, tmp, C);
        end = MPI_Wtime();

        comp_time += end - start;

        // shift a rows left
        start = MPI_Wtime();
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < j; k++) {
                // receive from right
                MPI_Irecv(tmpA, n_per_proc*n_per_proc, MPI_DOUBLE, right, 0, MPI_COMM_WORLD, &request);
                // send to left
                MPI_Send(local_subA, n_per_proc*n_per_proc, MPI_DOUBLE, left, 0, MPI_COMM_WORLD);

                // wait for the recv to finish
                MPI_Wait(&request, &status);
                // move new matrix into place
                memcpy(local_subA, tmpA, sizeof(double)*n_per_proc*n_per_proc);
            }
        }

        // shift b rows up
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < j; k++) {
                // receive from below
                MPI_Irecv(tmpB, n_per_proc*n_per_proc, MPI_DOUBLE, down, 0, MPI_COMM_WORLD, &request);
                // send up
                MPI_Send(local_subB, n_per_proc*n_per_proc, MPI_DOUBLE, up, 0, MPI_COMM_WORLD);

                // wait for the recv to finish
                MPI_Wait(&request, &status);
                // move new matrix into place
                memcpy(local_subB, tmpB, sizeof(double)*n_per_proc*n_per_proc);
            }
        }

        end = MPI_Wtime();
        comm_time += end - start;
    }

    if (rank == 0) {
        total_time = comm_time + comp_time;

        printf("comm: %.10lf seconds\n", comm_time);
        printf("comp: %.10lf seconds\n", comp_time);
        printf("total: %.10lf seconds\n", total_time);
    }

    MPI_Finalize();
    return 0;
}
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

void print_matrix(int rank, int height, int width, double** matrix)
{
    printf("mat rank: %d[\n", rank);
    for(int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i == 0 && j == 0) {
                printf("%lf", matrix[i][j]);
            }else {
                printf(" %lf", matrix[i][j]);
            }
        }
        printf("\n");
    }
    printf("]\n\n");
}

void print_vector(int length, double* vector) {
    printf("vec[");
    for(int i = 0; i < length; i++) {
        if (i == 0) {
            printf("%lf", vector[i]);
        }else {
            printf(" %lf", vector[i]);
        }
    }
    printf("]\n\n");
}

void print_scatter_response(int rank, int n_per_proc, double* vector) {
    printf("scat rank: %d[\n", rank);
    for(int i = 0; i < n_per_proc; i++) {
        for (int j = 0; j < n_per_proc; j++) {
            printf("%lf ", vector[i*n_per_proc + j]);
        }
        printf("\n");
    }
    printf("]\n\n");
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
            P = p*p*p;
        }else if (strmatch(arg, "-P")) {
            i += 1;
            P = atoi(argv[i]);
            p = (int) cbrt(P);
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
    for (int i = 0; i < p*p; i++) {
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
    double* ret = malloc(sizeof(double)*p*p*n_per_proc*n_per_proc);

    for (int i = 0; i < p*p; i++) {
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

    // these are funky but work :)
    int n_per_proc = n / p;
    int layer = (rank/(p*p))%p;
    int col = rank%p;
    int row = (rank - layer*p*p - col) / p;
    int layer_to_layer_color = row*p+col;
    int internal_rows = layer*p+row;
    int internal_cols = layer*p+col;

    // get all our comms
    // comms are as follows
    // layer_comm - Communicating to procs within a layer (all procs in the comm are in the same layer)
    // layer_to_layer_comm - Communication to procs with same row, col in a different layer (connects vertically through k)
    // internal_row_comm - Communication with other procs in your row interal to a layer (all procs are in same row and layer) 
    // internal_col_comm - Communication with other procs in your col interal to a layer (all procs are in same col and layer) 
    MPI_Comm layer_comm, layer_to_layer_comm, internal_row_comm, internal_col_comm;
    MPI_Comm_split(MPI_COMM_WORLD, layer, rank, &layer_comm);
    MPI_Comm_split(MPI_COMM_WORLD, layer_to_layer_color, rank, &layer_to_layer_comm);

    // splits each layer into its rows and columns (for propogation)
    MPI_Comm_split(MPI_COMM_WORLD, internal_rows, rank, &internal_row_comm);
    MPI_Comm_split(MPI_COMM_WORLD, internal_cols, rank, &internal_col_comm);

    // matrix generation
    if (rank == 0) {
        A = generate_matrix(n, 1);
        B = generate_matrix(n, 1);
        sub_matricesA = gen_submatrices(A);
        sub_matricesB = gen_submatrices(B);

        // this is a bit wasteful but I did it the first way first and its staying like that until I wanna fix it
        // which I probably never will
        scatterable_subA = oneDify(sub_matricesA);
        scatterable_subB = oneDify(sub_matricesB);
    }

    // load up the bottom layer with the data
    double* local_subA = malloc(sizeof(double)*n_per_proc*n_per_proc);
    double* local_subB = malloc(sizeof(double)*n_per_proc*n_per_proc);

    start = MPI_Wtime();
    if(layer == 0) {
        MPI_Scatter(scatterable_subA, n_per_proc*n_per_proc, MPI_DOUBLE, local_subA, n_per_proc*n_per_proc, MPI_DOUBLE, 0, layer_comm);
        MPI_Scatter(scatterable_subB, n_per_proc*n_per_proc, MPI_DOUBLE, local_subB, n_per_proc*n_per_proc, MPI_DOUBLE, 0, layer_comm);
    }
    end = MPI_Wtime();
    comm_time += end-start;

    // now we need to get the rows to the right layer
    // big ol if statement
    // basically every proc will send up if it needs to
    MPI_Status a;
    MPI_Status b;
    start = MPI_Wtime();
    if (layer > 0) {
        if (layer == row) MPI_Recv(local_subA, n_per_proc*n_per_proc, MPI_DOUBLE, row*p + col, 0, MPI_COMM_WORLD, &a);
        if (layer == col) MPI_Recv(local_subB, n_per_proc*n_per_proc, MPI_DOUBLE, row*p + col, 1, MPI_COMM_WORLD, &b);
    }else {
        if (row > 0) MPI_Send(local_subA, n_per_proc*n_per_proc, MPI_DOUBLE, row*p*p + row*p + col, 0, MPI_COMM_WORLD);
        if (col > 0) MPI_Send(local_subB, n_per_proc*n_per_proc, MPI_DOUBLE, col*p*p + row*p + col, 1, MPI_COMM_WORLD);
    }
    end = MPI_Wtime();
    comm_time += end-start;
    
    // now we propgate to the other rows/cols in the layer
    start = MPI_Wtime();
    MPI_Bcast(local_subA, n_per_proc*n_per_proc, MPI_DOUBLE, layer, internal_col_comm);
    MPI_Bcast(local_subB, n_per_proc*n_per_proc, MPI_DOUBLE, layer, internal_row_comm);
    end = MPI_Wtime();
    comm_time += end-start;

    // multiply locally
    start = MPI_Wtime();
    double* local_C = matrix_mult(n_per_proc, local_subA, local_subB);
    end = MPI_Wtime();
    comp_time += end-start;

    double* C = malloc(sizeof(double)*n_per_proc*n_per_proc);

    // add all the stuff up
    start = MPI_Wtime();
    MPI_Allreduce(local_C, C, n_per_proc*n_per_proc, MPI_DOUBLE, MPI_SUM, layer_to_layer_comm);
    end = MPI_Wtime();
    comm_time += end-start;

    // gather all the Cs to proc 0
    double* all_of_c = NULL;
    if (rank == 0) all_of_c = malloc(sizeof(double)*n*n);
    start = MPI_Wtime();
    if (layer == 0) MPI_Gather(C, n_per_proc*n_per_proc, MPI_DOUBLE, all_of_c, n_per_proc*n_per_proc, MPI_DOUBLE, 0, layer_comm);
    end = MPI_Wtime();
    comm_time += end-start;
    if (rank == 0) {
        total_time = comm_time + comp_time;

        printf("comm: %.10lf seconds\n", comm_time);
        printf("comp: %.10lf seconds\n", comp_time);
        printf("total: %.10lf seconds\n", total_time);
    }



    // massive stack of frees
    if (all_of_c) free(all_of_c);
    if (C) free(C);
    if (local_C) free(local_C);
    if (local_subA) free(local_subA);
    if (local_subB) free(local_subB);
    if (scatterable_subA) free(scatterable_subA);
    if (scatterable_subB) free(scatterable_subB);
    if (sub_matricesA || sub_matricesB) {
        for (int i = 0; i < p*p; i++) {
            for (int j = 0; j < n_per_proc; j++) {
                free(sub_matricesA[i][j]);
                free(sub_matricesB[i][j]);
            }
            free(sub_matricesA[i]);
            free(sub_matricesB[i]);
        }
        free(sub_matricesA);
        free(sub_matricesB);
    }

    if (A || B) {
        for(int i = 0; i < n; i++) {
            free(A[i]);
            free(B[i]);
        }
        free(A);
        free(B);
    }

    MPI_Finalize();
    return 0;
}
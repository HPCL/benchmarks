/*
 * Matrix Multiply.
 *
 * This is a simple matrix multiply program which will compute the product
 *
 *                C  = A * B
 *
 * A ,B and C are both square matrix. They are statically allocated and
 * initialized with constant number, so we can focus on the parallelism.
 *
 * usage: mpirun -np <N> mm_mpi [-t]
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "mpi.h"



#ifdef USE_CALI
#include <caliper/cali.h>
#include <caliper/cali-mpi.h>
#endif

#ifndef ORDER
#define ORDER 1000   // the order of the matrix
#endif
#define AVAL  3.0    // initial value of A
#define BVAL  5.0    // initial value of B
#define TOL   0.001  // tolerance used to check the result

#define TYPE double

#define TRUE  1
#define FALSE 0

MPI_Status status;

// Initialize the matrices (uniform values to make an easier check)
void matrix_init(TYPE** A, TYPE** B, TYPE** C, size_t size) {
  size_t i, j;

  *A = (TYPE*)malloc(size*size*sizeof(TYPE));
  *B = (TYPE*)malloc(size*size*sizeof(TYPE));
  *C = (TYPE*)malloc(size*size*sizeof(TYPE));

  for (j=0; j<size*size; j++) {
    *A[j] = AVAL;
    *B[j] = BVAL;
    *C[j] = 0.0;
  }
 
}

void transpose_mat(TYPE* A) {

  TYPE temp;
  for (int i=1; i<ORDER; i++) {
    for (int j=i; j<ORDER; j++) {
      temp = A[i*ORDER+j];
      A[i*ORDER+j]=A[j*ORDER+i];
      A[j*ORDER+i]=temp;
    }
  }
}

void matrix_free(TYPE* A, TYPE* B, TYPE* C, size_t size) {
  size_t i;

  free(A);
  free(B);
  free(C);

}

// The actual mulitplication function, totally naive
double row_multiply(TYPE* A, size_t row_a, TYPE* B, size_t col_b, TYPE* out_buffer) {

  size_t k;
  double start, end;

  for (k=0; k<ORDER; k++){
    out_buffer[row_a*ORDER+col_b] += A[row_a*ORDER+k] * B[k*ORDER+col_b];
  }
  
  return 0.0;

}

// The actual mulitplication function, totally naive
double row_multiply_T(TYPE* A, size_t row_a, TYPE* B, size_t col_b, TYPE* out_buffer) {

  size_t k;
  double start, end;

  for (k=0; k<ORDER; k++){
    out_buffer[row_a*ORDER+col_b] += A[row_a*ORDER+k] * B[col_b*ORDER+k];
  }
  
  return 0.0;

}

void print_mat(TYPE* C) {
  
  size_t i, j;
  double e  = 0.0;
  double ee = 0.0;
  double v  = AVAL * BVAL * ORDER;

  for (i=0; i<ORDER; i++) {
    for (j=0; j<ORDER; j++) {
      printf("%f    ",C[i*ORDER+j]);
    }
    printf("\n\n");
  }

}

// Function to check the result, relies on all values in each initial
// matrix being the same
size_t check_result(TYPE* C) {
        size_t i, j;

        double e  = 0.0;
        double ee = 0.0;
        double v  = AVAL * BVAL * ORDER;

        for (i=0; i<ORDER; i++) {
          for (j=0; j<ORDER; j++) {
            e = C[i*ORDER+j] - v;
            ee += e * e;
          }
        }
        if (ee > TOL) {
          return 0;
        } else {
          return 1;
        }
}

// main function
int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  size_t order_2 = (size_t)ORDER*(size_t)ORDER;

  size_t correct;
  size_t rc = 0;
  double run_time;
  double mflops;
	
  int num_ranks, rank_id, rows_per_rank;
  size_t i,j,k,r;
  size_t master_rows, master_i;

  double start, end;

  TYPE *A, *B, *C, *out_buffer;

  MPI_Status status;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank_id);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  char do_transpose = FALSE;

  if (argc > 1) {
    if ( !(strcmp("-t", argv[1])) || !(strcmp("--transpose", argv[1])) )
      do_transpose = TRUE;
  }



#ifdef USE_CALI
cali_mpi_init();
#endif

  if (num_ranks == 1) {
    fprintf(stderr,"serial not supported\n");
    return 1;
  }
  rows_per_rank = ORDER/(num_ranks-1); // TODO make it work with one process

  if (rank_id == 0) { // master

    //size_t nt = omp_get_max_threads();
    printf("Available processes =  %d \n", num_ranks);

    // initialize the matrices
    printf("init...\n");
    master_rows =  ORDER%(num_ranks-1);
    master_i = ORDER*(ORDER-master_rows);
    A = (TYPE*)malloc(order_2*sizeof(TYPE));
    B = (TYPE*)malloc(order_2*sizeof(TYPE));
    C = (TYPE*)malloc(order_2*sizeof(TYPE));
    out_buffer = (TYPE*)malloc(master_rows*ORDER*sizeof(TYPE));

    for (j=0; j<order_2; j++) {
      A[j] = AVAL;
      B[j] = BVAL;
      C[j] = 0.0;
    }

    if(do_transpose) {
      printf("Transposing B matrix\n");
      transpose_mat(B);
    }

    /*
    printf("\n\n");
    printf("rows_per_rank = %d\n", rows_per_rank);
    printf("order         = %d\n", ORDER);
    printf("num_ranks     = %d\n", num_ranks);
    printf("\n\n");
    print_mat(A);
    printf("\n\n");
    print_mat(B);
    printf("\n\n");
    print_mat(C);
    printf("\n\n");
    */

    printf("multiply...\n");
    start = omp_get_wtime();

    //send
    for(r = 1; r < num_ranks; r++){
      MPI_Send(&A[(r-1)*rows_per_rank*ORDER], rows_per_rank*ORDER, MPI_DOUBLE, r, 0, MPI_COMM_WORLD);
      MPI_Send(B, order_2, MPI_DOUBLE, r, 1, MPI_COMM_WORLD);  
    }

#ifdef USE_CALI
CALI_MARK_BEGIN("master_mult");
#endif

    if(do_transpose) {
      //multiply
      for(i = ORDER-master_rows; i < ORDER; i++){
        for(j = 0; j < ORDER; j++){
          #pragma simd
          for(k = 0; k < ORDER; k++) {
              C[i*ORDER+j] += A[i*ORDER+k] * B[j*ORDER+k];
          }
        }
      }
    } else {
      for(i = ORDER-master_rows; i < ORDER; i++){
        for(j = 0; j < ORDER; j++){
          #pragma simd
          for(k = 0; k < ORDER; k++) {
              C[i*ORDER+j] += A[i*ORDER+k] * B[k*ORDER+j];
          }
        }
      }
    } //transpose

#ifdef USE_CALI
CALI_MARK_END("master_mult");
#endif

    //recieve
    for(r = 1; r < num_ranks; r++){
      MPI_Recv(&C[(r-1)*rows_per_rank*ORDER], rows_per_rank*ORDER, MPI_DOUBLE, r, 2, MPI_COMM_WORLD, &status);
    }

    end = omp_get_wtime();

    // verify that the result is sensible
    printf("check...\n");
    correct  = check_result(C);

    //printf("\n\n");    
    //print_mat(C);
    //printf("\n\n");

    // Compute the number of mega flops
    run_time = end - start;
    mflops = (2.0 * order_2*ORDER) / (1000000.0 * run_time);
    printf("Order %d multiplication in %f seconds \n", ORDER, run_time);
    printf("Order %d multiplication at %f mflops\n", ORDER, mflops);

    // Check results
    if (! correct) {
      fprintf(stderr,"\n Errors in multiplication\n");
      rc = 1;
    } else {
      fprintf(stdout,"\n SUCCESS : results match\n");
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    matrix_free(A,B,C,ORDER);
    free(out_buffer);

  } else { // workers
    
    //alloc
    out_buffer = (TYPE*)malloc(rows_per_rank*ORDER*sizeof(TYPE));
    A = (TYPE*)malloc(rows_per_rank*ORDER*sizeof(TYPE));
    B = (TYPE*)malloc(order_2*sizeof(TYPE));
    for(i = 0; i < rows_per_rank*ORDER; i++) out_buffer[i]=0;

    //recieve
    MPI_Recv(A, rows_per_rank*ORDER, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(B, order_2, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);


#ifdef USE_CALI
CALI_MARK_BEGIN("worker_mult");
#endif

    if(do_transpose) {
      //multiply
      for(i = 0; i < rows_per_rank; i++){
        for(j = 0; j < ORDER; j++){
          #pragma simd
          #pragma ivdep
          for (k=0; k<ORDER; k++){
            out_buffer[i*ORDER+j] += A[i*ORDER+k] * B[j*ORDER+k];
          }
        }
      }
    } else {
      for(i = 0; i < rows_per_rank; i++){
        for(j = 0; j < ORDER; j++){
          #pragma simd
          #pragma ivdep
          for (k=0; k<ORDER; k++){
            out_buffer[i*ORDER+j] += A[i*ORDER+k] * B[k*ORDER+j];
          }
        }
      }

    } // tranpose


#ifdef USE_CALI
CALI_MARK_END("worker_mult");
#endif

    //send
    MPI_Send(out_buffer, rows_per_rank*ORDER, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    free(out_buffer);
    free(A);
    free(B);
  }

  MPI_Finalize();

  return rc;
}


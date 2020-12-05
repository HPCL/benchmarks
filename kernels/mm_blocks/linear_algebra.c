/*
 * linear_algebra.c
 * simple code for matrix funcitons
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * See LICENSE file for licensing information and boring legal stuff

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate, coffee, or introductions to potential employers.

*/

#include "linear_algebra.h"
#include <math.h>
 
 
//add matrices together
//pre all matrices are initialized, c shouldn't have any important data in it
//     all matrices should be the same dimensions
//post mat_c has the result of multipling mat_a and mat_b
void add_matrix(double** mat_a, int rows, int cols, double** mat_b, double** mat_c) {

  int i, j;
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      mat_c[i][j] = mat_a[i][j] + mat_b[i][j];
    }
  }

}

//multiply matrices together
//pre all matrices are initialized, c shouldn't have any important data in it
//     rows in b == cols in a
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix(double** mat_a, int rows_a, int cols_a, 
                     double** mat_b, int cols_b, 
                     double** mat_c) {

  int i, j, k;

  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_b; j++) {
      mat_c[i][j] = 0;
      for (k = 0; k < cols_a; k++) {
        mat_c[i][j] += mat_a[i][k] * mat_b[k][j];
      }
    } 
  }

}

//multiply matrices together
//pre all matrices are initialized, c shouldn't have any important data in it
//     mat b is trnasposed
//     rows in b == cols in a
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix_t(double** restrict mat_a, int rows_a, int cols_a, 
                       double** restrict mat_b, int cols_b, 
                       double** restrict mat_c,
                       int block_size) {

  int i, j, k;
  int ii, jj;

  int remainder =  cols_b%block_size;
  int block_cols = cols_b-remainder;

#pragma omp parallel private(i,j,k,ii,jj)
{
#ifdef USE_CALI
CALI_MARK_BEGIN("block_mm");
#endif

  #pragma omp for
  for (i = 0; i < rows_a; i++) {
  // for (ii = 0; ii < block_rows; ii+=block_size) {
  // for (i = ii; i < ii+block_size; i++) {
    for (jj = 0; jj < block_cols; jj+=block_size) {
    #pragma omp simd
    for (j = jj; j < jj+block_size; j++) {
      mat_c[i][j] = 0;
      #pragma unroll
      for (k = 0; k < cols_a; k++) {
        mat_c[i][j] += mat_a[i][k] * mat_b[j][k];
      }
    } 
    }
    #pragma omp simd
    for (j =  block_cols; j < cols_b; j++) {
      mat_c[i][j] = 0;
      #pragma unroll
      for (k = 0; k < cols_a; k++) {
        mat_c[i][j] += mat_a[i][k] * mat_b[j][k];
      }
    } 
  }

  // #pragma omp for
  // for (i = block_rows; i < remainder; i++) {
  // }


#ifdef USE_CALI
CALI_MARK_END("block_mm");
#endif
}

}

//transpose a matrix
//pre all matrices are initialized, c shouldn't have any important data in it
//     rows in c == cols in a
//post mat_c has the transpose of mat_a
void transpose_matrix(double** mat_a, int rows_a, int cols_a, double** mat_c) {
  int i, j;

  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_a; j++) {
      mat_c[j][i] = mat_a[i][j];
    }
  }
}

//set a matrix to zero
//pre matrix_a has been allocated to rows_a x cols_a
//post mat_a is all zeros
void set_zero(double** mat_a, int rows_a, int cols_a) {
  int i, j;

  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_a; j++) {
      mat_a[i][j] = 0;
    }
  }
}

//set a matrix to the identity
//pre matrix_a has been allocated to rows_a x cols_a
//post mat_a has ones in the diagonal and zeros elsewhere
void set_identity(double** mat_a, int rows_a, int cols_a) {
  int i, j;

  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_a; j++) {
      mat_a[i][j] = (double)(i == j);
    }
  }
}

//deep copy of a to b
void copy_mat(double** mat_a, double** mat_c, int total_elms) {
  int i;
  for (i = 0; i < total_elms; i++)
    mat_c[i] = mat_a[i];
}


void print_matrix(double** mat_a, int rows_a, int cols_a) {

  int i, j;

  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_a; j++) {
      printf("%.4f ", mat_a[i][j]);
    }
    printf("\n\n");
  }
}

//returns abs(a)
//TODO make a macro?
double get_abs(double a) {
  return (((a < 0.) * -2.) + 1.) * a;
  
}



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
                       double** restrict mat_c) {

  int i, j, k;
  int ii, jj, kk;
  int iii, jjj, kkk;

#ifdef USE_LIKWID
#pragma omp parallel
{
LIKWID_MARKER_START("block_mm");
}
#endif

#ifdef USE_CALI_UNCORE
CALI_MARK_BEGIN("block_mm");
#endif

#pragma omp parallel private(iii,jjj,ii,jj,i,j,k)
{
#ifdef USE_CALI_REG
CALI_MARK_BEGIN("block_mm");
#endif

  #pragma omp for
  for (int i = 0; i < rows_a; i++ ) {
    for (int jjj = 0; jjj < cols_b; jjj = jjj + BLOCK_ROWS) 
      for (int j = jjj; j < min(cols_b, jjj + BLOCK_ROWS); j++) 
        for (int kkk = 0; kkk < cols_a; kkk = kkk + BLOCK_COLS) {
          for (int k = kkk; k < min(cols_a,kkk + BLOCK_COLS); k++)
            mat_c[i][j] = mat_c[i][j] + mat_a[i][k] * mat_b[j][k];
        }
  } // i

#ifdef USE_CALI_REG
CALI_MARK_END("block_mm");
#endif
}

#ifdef USE_CALI_UNCORE
CALI_MARK_END("block_mm");
#endif

#ifdef USE_LIKWID
#pragma omp parallel
{
LIKWID_MARKER_STOP("block_mm");
}
#endif

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



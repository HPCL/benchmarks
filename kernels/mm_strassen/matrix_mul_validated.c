/*
 * matrix_mul_validated.c
 * simple code to validate alternative matrix multiplication functions
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * See LICENSE file for licensing information

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate, coffee, or introductions to potential employers.

*/

#include "linear_algebra.h"
#include <math.h>
 
//multiply matrices together
//pre all matrices are initialized, c shouldn't have any important data in it
//     mat b is trnasposed
//     rows in b == cols in a
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix_v(double** mat_a, int rows_a, int cols_a, 
                       double** mat_b, int cols_b, 
                       double** mat_c) {

  int i, j, k;

  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_b; j++) {
      mat_c[i][j] = 0;
      for (k = 0; k < cols_a; k++) {
        mat_c[i][j] += mat_a[i][k] * mat_b[j][k];
      }
    } 
  }

}


void multiply_matrix_vp(double** mat_a, int rows_a, int cols_a, 
                       double** mat_b, int cols_b, 
                       double** mat_c) {

  int i, j, k;
  #pragma omp parallel for private(i,j,k)
  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_b; j++) {
      mat_c[i][j] = 0;
      for (k = 0; k < cols_a; k++) {
        mat_c[i][j] += mat_a[i][k] * mat_b[j][k];
      }
    } 
  }

}




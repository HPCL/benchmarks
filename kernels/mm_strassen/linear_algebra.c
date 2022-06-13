/*
 * linear_algebra.c
 * simple code for matrix funcitons

 * Brian J Gravelle

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

//multiply matrices together, serial
//pre all matrices are initialized, c shouldn't have any important data in it
//     rows in b == cols in a
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix_s(double** mat_a, int rows_a, int cols_a,
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
//this version uses the strassen algorthim
//pre all matrices are initialized, c shouldn't have any important data in it
//     mat b is NOT transposed
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix(double** mat_a, int rows_a, int cols_a,
                       double** mat_b, int cols_b,
                       double** mat_c) {

  // for now make sure everything is square and power of two
  if(!( (rows_a == cols_a) && (rows_a == cols_b) && (cols_b == cols_a) )) {
    printf("\n\nError: matrix not square\n"); fflush(stdout);
    exit(1);
  }
  // check if power of two
  // TODO padding if this isn't the case
  // TODO maybe chack and pad prior to calling recursion
  if( !(rows_a && (!(rows_a & (rows_a - 1)))) ) {
    printf("\n\nError: matrix not power of 2\n"); fflush(stdout);
    exit(1);
  }

  #pragma omp parallel
  #pragma omp single nowait
  multiply_matrix_strassen(mat_a, rows_a, rows_a,
                           mat_b, mat_c,
                           0, 0,
                           0, 0);

}


//multiply matrices together
//this version uses the recursive strassen algorthim
//pre all matrices are initialized, c shouldn't have any important data in it
//     mat b is NOT transposed
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix_strassen(double** mat_a, int order, int current_size,
                              double** mat_b, double** mat_c,
                              int start_row_a, int start_col_a,
                              int start_row_b, int start_col_b) {

   // base case
   // multiply two numbers or whatever
   // TODO make this cover larger matrices
   if(current_size == 1) {

       mat_c[0][0] = mat_a[start_row_a][start_col_a] * mat_b[start_row_b][start_col_b];

  } else if(current_size == 4) {

        for (size_t i = 0; i < current_size; i++)
            for (size_t j = 0; j < current_size; j++)
                for (size_t k = 0; k < current_size; k++)
      mat_c[i][j] += mat_a[start_row_a+i][start_col_a+k] * mat_b[start_row_b+k][start_col_b+j];

   } else {
    // general case
    // split matrix into 4
    int split_size = current_size/2;

    // assume the matrices are broken down as shown:
    // A => a1 a2
    //      a3 a4
    // B => b1 b2
    //      b3 b4
    // then C is:
    //    a1*b1+a2*b3  a1*b2+a2*b4
    //    a3*b1+a4*b3  a3*b2+a4*b4

    // upper left
    double** a1b1 = (double**)malloc(split_size*sizeof(double*));
    double** a2b3 = (double**)malloc(split_size*sizeof(double*));
    double** a1b2 = (double**)malloc(split_size*sizeof(double*));
    double** a2b4 = (double**)malloc(split_size*sizeof(double*));
    double** a3b1 = (double**)malloc(split_size*sizeof(double*));
    double** a4b3 = (double**)malloc(split_size*sizeof(double*));
    double** a3b2 = (double**)malloc(split_size*sizeof(double*));
    double** a4b4 = (double**)malloc(split_size*sizeof(double*));
    for (int i=0; i<split_size; i++) {
      a1b1[i] = (double*)malloc(split_size*sizeof(double));
      a2b3[i] = (double*)malloc(split_size*sizeof(double));
      a1b2[i] = (double*)malloc(split_size*sizeof(double));
      a2b4[i] = (double*)malloc(split_size*sizeof(double));
      a3b1[i] = (double*)malloc(split_size*sizeof(double));
      a4b3[i] = (double*)malloc(split_size*sizeof(double));
      a3b2[i] = (double*)malloc(split_size*sizeof(double));
      a4b4[i] = (double*)malloc(split_size*sizeof(double));
    }
    // TODO make this a loop
    // maybe make a list of the temp arrays
    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a1b1,
                             start_row_a, start_col_a,
                             start_row_b, start_col_b); //

    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a2b3,
                             start_row_a, start_col_a+split_size,
                             start_row_b+split_size, start_col_b); //

    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a1b2,
                             start_row_a, start_col_a,
                             start_row_b, start_col_b+split_size);  //

    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a2b4,
                             start_row_a, start_col_a+split_size,
                             start_row_b+split_size, start_col_b+split_size); //

    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a3b1,
                             start_row_a+split_size, start_col_a,
                             start_row_b, start_col_b);

    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a4b3,
                             start_row_a+split_size, start_col_a+split_size,
                             start_row_b+split_size, start_col_b);

    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a3b2,
                             start_row_a+split_size, start_col_a,
                             start_row_b, start_col_b+split_size);

    #pragma omp task shared(mat_a,mat_b)
    multiply_matrix_strassen(mat_a, order, split_size,
                             mat_b, a4b4,
                             start_row_a+split_size, start_col_a+split_size,
                             start_row_b+split_size, start_col_b+split_size);

    #pragma omp taskwait
    // fill c matrix from temp
    // then C is:
    //    a1*b1+a2*b3  a1*b2+a2*b4
    //    a3*b1+a4*b3  a3*b2+a4*b4
    #pragma omp parallel for
    for(int i = 0; i < split_size; i++) {
      for(int j = 0; j < split_size; j++) {
        mat_c[i][j]                       = a1b1[i][j] + a2b3[i][j];
        mat_c[i][j+split_size]            = a1b2[i][j] + a2b4[i][j];
        mat_c[i+split_size][j]            = a3b1[i][j] + a4b3[i][j];
        mat_c[i+split_size][j+split_size] = a3b2[i][j] + a4b4[i][j];
      }
    }


    for (int i=0; i<split_size; i++) {
      free(a1b1[i]);
      free(a2b3[i]);
      free(a1b2[i]);
      free(a2b4[i]);
      free(a3b1[i]);
      free(a4b3[i]);
      free(a3b2[i]);
      free(a4b4[i]);
    }
    free(a1b1);
    free(a2b3);
    free(a1b2);
    free(a2b4);
    free(a3b1);
    free(a4b3);
    free(a3b2);
    free(a4b4);

  } // else; general case

}

//transpose a matrix
//pre all matrices are initialized, c shouldn't have any important data in it
//     rows in c == cols in a
//post mat_c has the transpose of mat_a
void transpose_matrix(double** mat_a, int rows_a, int cols_a, double** mat_c) {

  #pragma omp parallel for
  for (int i = 0; i < rows_a; i++) {
    for (int j = 0; j < cols_a; j++) {
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
void copy_mat(double** mat_a, double** mat_c, int rows_a, int cols_a) {
  int i,j;

  for (i = 0; i < rows_a; i++) {
    for (j = 0; j < cols_a; j++) {
      mat_c[i][j] = mat_a[i][j];
    }
  }
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

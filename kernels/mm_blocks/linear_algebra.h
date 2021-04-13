/*
 * linear_algebra.h
 * simple header for matrix function code
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * credit to www.mathwords.com for definitions and help with linear algebra

 * See LICENSE file for licensing information and boring legal stuff

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate, coffee, or introductions to potential employers.

*/

#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#ifdef USE_LIKWID
#include <likwid.h>
#include <likwid-marker.h>
#endif

#ifndef ORDER
#define ORDER 2048
#endif
#ifndef BLOCK_ROWS
#define BLOCK_ROWS 256
#endif
#ifndef BLOCK_COLS
#define BLOCK_COLS 256
#endif

#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))

void print_matrix(double** mat_a, int rows_a, int cols_a);
 
//add matrices together
//pre all matrices are initialized, c shouldn't have any important data in it
//     all matrices should be the same dimensions
//post mat_c has the result of multipling mat_a and mat_b
void add_matrix(double** mat_a, int rows, int cols, double** mat_b, double** mat_c);

//multiply matrices together
//pre all matrices are initialized, c shouldn't have any important data in it
//     rows in b == cols in a
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix_s(double** mat_a, int rows_a, int cols_a, double** mat_b, int cols_b, double** mat_c);
// same but with b transposed
void multiply_matrix_d(double** restrict __attribute__((aligned (64))) mat_a, int rows_a, int cols_a, 
                       double** restrict __attribute__((aligned (64))) mat_b, int cols_b, 
                       double** restrict __attribute__((aligned (64))) mat_c);
// loop interchanged
void multiply_matrix_i(double** restrict __attribute__((aligned (64))) mat_a, int rows_a, int cols_a, 
                       double** restrict __attribute__((aligned (64))) mat_b, int cols_b, 
                       double** restrict __attribute__((aligned (64))) mat_c);
// b transposed
void multiply_matrix_t(double** restrict __attribute__((aligned (64))) mat_a, int rows_a, int cols_a, 
                       double** restrict __attribute__((aligned (64))) mat_b, int cols_b, 
                       double** restrict __attribute__((aligned (64))) mat_c);
// same but with b transposed and blocked
void multiply_matrix_b(double** restrict __attribute__((aligned (64))) mat_a, int rows_a, int cols_a, 
                       double** restrict __attribute__((aligned (64))) mat_b, int cols_b, 
                       double** restrict __attribute__((aligned (64))) mat_c);
// b transposed with different loops flipped blocked
void multiply_matrix_f(double** restrict __attribute__((aligned (64))) mat_a, int rows_a, int cols_a, 
                       double** restrict __attribute__((aligned (64))) mat_b, int cols_b, 
                       double** restrict __attribute__((aligned (64))) mat_c);

// validated matrix multiply
void multiply_matrix_v(double** restrict __attribute__((aligned (64))) mat_a, int rows_a, int cols_a, 
                       double** restrict __attribute__((aligned (64))) mat_b, int cols_b, 
                       double** restrict __attribute__((aligned (64))) mat_c);
// validated matrix multiply in parallel
void multiply_matrix_vp(double** restrict __attribute__((aligned (64))) mat_a, int rows_a, int cols_a, 
                       double** restrict __attribute__((aligned (64))) mat_b, int cols_b, 
                       double** restrict __attribute__((aligned (64))) mat_c);

//transpose a matrix
//pre all matrices are initialized, c shouldn't have any important data in it
//     rows in c == cols in a
//post mat_c has the transpose of mat_a
void transpose_matrix(double** mat_a, int rows_a, int cols_a, double** mat_c);

//set a matrix to zero
//pre matrix_a has been allocated to rows_a x cols_a
//post mat_a is all zeros
void set_zero(double** mat_a, int rows_a, int cols_a);

//set a matrix to the identity
//pre matrix_a has been allocated to rows_a x cols_a
//post mat_a has ones in the diagonal and zeros elsewhere
void set_identity(double** mat_a, int rows_a, int cols_a);

//deep copy of a to c
void copy_mat(double** mat_a, double** mat_c, int total_elms);

//returns abs(a)
double get_abs(double a);



#endif

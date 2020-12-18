/*
 * test_linear_algebra.c
 * a small program to test the linear algebra code
 
 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * See LICENSE file for licensing information and boring legal stuff

 * If by some miricale you find this software useful, thanks are accepted in
 * the form of chocolate or introductions to potential employers.

*/

#include "linear_algebra.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void test_multiply();
void test_multiply_2(int block_size);
void test_add();
void test_transpose();

int main(int argc, char **argv) {

  char temp[16];
  int block_size = 128;
  if (argc > 1)
    block_size = atoi(argv[1]);

  // omp_set_num_threads(48);

  #ifdef USE_CALI
  cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
  #pragma omp parallel
  {
  cali_set_int(thread_attr, omp_get_thread_num());
  }
  #endif

  #pragma omp parallel
  {
  if (omp_get_thread_num() == 1)
    printf("Number of threads: %d\n",omp_get_num_threads());
  }
  
  // printf("Enter 'c' to continue. Note it may require multiple entries.\n");
  // scanf("%s", temp);
  // test_add();
  // scanf("%s", temp);
  // test_transpose();
  // scanf("%s", temp);
  // test_multiply();
  test_multiply_2(block_size);
  // scanf("%s", temp);
  printf("Bye now!\n\n");

  return 0;
}

void test_add() {
  int col_A = 2, row_A = 3;
  double A[3][2] = {{2,2},
                  {2,2},
                  {2,2}};
                
  int col_B = 2, row_B = 3;
  double B[3][2] = {{2,2},
                  {2,2},
                  {2,2}};
                
  int col_C = 2, row_C = 3;
  double C[3][2] = {{2,2},
                  {2,2},
                  {2,2}};


  printf("\nA:\n");
  print_matrix((double**)A, row_A, col_A);
  printf("\nB:\n");
  print_matrix((double**)B, row_B, col_B);
  printf("\nC:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");

  add_matrix((double**)A, row_A, col_A, (double**)B, (double**)C);
  printf("\nC <- A + B:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");
}

void test_multiply() {
  int col_A = 3, row_A = 3;
  double A[3][3] = {{1,2,3},
                    {4,5,6},
                    {7,8,9}};
                
  int col_B = 2, row_B = 3;
  double B[3][2] = {{1,2},
                    {3,4},
                    {5,6}};
                
  int col_C = 2, row_C = 3;
  double C[3][2] = {{2,2},
                    {2,2},
                    {2,2}};


  printf("\nA:\n");
  print_matrix((double**)A, row_A, col_A);
  printf("\nB:\n");
  print_matrix((double**)B, row_B, col_B);
  printf("\nC:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");

  multiply_matrix((double**)A, row_A, col_A, (double**)B, col_B, (double**)C);
  printf("\nC <- A * B:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");
}

void test_multiply_2(int block_size) {
  int size = 17;
#ifdef ORDER
  size = ORDER;
#endif

  printf("Running blocked matrix multiply.\n");
  printf("  Matrix side length: %d\n", size);
  printf("  Block size is:      %d\n", block_size);

  int i,j,k;
  double wall_start, wall_end;
  double e  = 0.0;
  double ee = 0.0;
  double a_val = 0.0;
  double b_val = 0.0;
  double v  = a_val * b_val * size;
  double** A;    
  double** B;    
  double** C;

  A = (double**)malloc(size*sizeof(double*));
  B = (double**)malloc(size*sizeof(double*));
  C = (double**)malloc(size*sizeof(double*));

  for (i=0; i<size; i++) {
    A[i] = (double*)malloc(size*sizeof(double));
    B[i] = (double*)malloc(size*sizeof(double));
    C[i] = (double*)malloc(size*sizeof(double));
  }

  for (i=0; i<size; i++) {
  for (j=0; j<size; j++) {
    A[i][j] = a_val;
    B[i][j] = b_val;
    C[i][j] = 0.0;
  }
  }


  printf("  sizeof A is:        %d\n", sizeof(A[0]));

  wall_start = omp_get_wtime();
  multiply_matrix_t(A, size, size, B, size, C, block_size);
  wall_end = omp_get_wtime();

  #pragma omp parallel for
  for (i=0; i<size; i++) {
    for (j=0; j<size; j++) {
      e = C[i][j] - v;
      ee += e * e;
    }
  }
  if (ee > 0.05) {
    printf("Multiply complete: Falied\n");
  } else {
    printf("Multiply complete: Success\n");
    printf("Time: %fs\n", (wall_end - wall_start));
  }


  for (i=0; i<size; i++) {
    free(A[i]);
    free(B[i]);
    free(C[i]);
  }
  free(A);
  free(B);
  free(C);
}

void test_transpose() {
  int col_A = 3, row_A = 4;
  double A[4][3] = {{1, 2, 3},
                {4, 5, 6},
                {7, 8, 9},
                {10,11,12}};
                                
  int col_C = 4, row_C = 3;
  double C[3][4] = {{2,2,2,2},
                {2,2,2,2},
                {2,2,2,2}};


  printf("\nA:\n");
  print_matrix((double**)A, row_A, col_A);
  printf("\nC:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");

  transpose_matrix((double**)A, row_A, col_A, (double**)C);
  printf("\nC <- A^T:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");
}

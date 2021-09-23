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
#include <time.h>

void test_multiply();
void test_multiply_2();
void test_add();
void test_transpose();

int main(int argc, char **argv) {

  char temp[16];
  int block_size = 128;
  if (argc > 1)
    block_size = atoi(argv[1]);

  // omp_set_num_threads(48);

#ifdef USE_CALI
  cali_init();
  cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);

#ifdef USE_CALI_REG
  #pragma omp parallel
  {
  cali_set_int(thread_attr, omp_get_thread_num());
  }
#endif
#ifdef USE_CALI_UNCORE
  cali_set_int(thread_attr, omp_get_thread_num());
#endif

#endif

  #pragma omp parallel
  {
  if (omp_get_thread_num() == 1)
    printf("Number of threads: %d\n",omp_get_num_threads());
  }
  

#ifdef USE_LIKWID
LIKWID_MARKER_INIT;
LIKWID_MARKER_REGISTER("block_mm");
#endif
  
  // printf("Enter 'c' to continue. Note it may require multiple entries.\n");
  // scanf("%s", temp);
  // test_add();
  // scanf("%s", temp);
  // test_transpose();
  // scanf("%s", temp);
  // test_multiply();
  test_multiply_2();
  // scanf("%s", temp);

#ifdef USE_LIKWID
LIKWID_MARKER_CLOSE;
#endif
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
  int col_B = 2, row_B = 3;
  int col_C = 2, row_C = 3;

  int i,j,k;

  double** A  = (double**)malloc(row_A*sizeof(double*));
  double** B  = (double**)malloc(row_B*sizeof(double*));
  double** Bt = (double**)malloc(col_B*sizeof(double*));
  double** C  = (double**)malloc(row_A*sizeof(double*));

  for (i=0; i<row_A; i++) A[i]  = (double*)malloc(col_A*sizeof(double));
  for (i=0; i<row_B; i++) B[i]  = (double*)malloc(col_B*sizeof(double));
  for (i=0; i<col_B; i++) Bt[i] = (double*)malloc(row_B*sizeof(double));
  for (i=0; i<row_A; i++) C[i]  = (double*)malloc(col_B*sizeof(double));
  
  for (i=0; i<row_A; i++)
  for (j=0; j<col_A; j++)
    A[i][j] = 1.0 + i + j;

  for (i=0; i<row_B; i++)
  for (j=0; j<col_B; j++)
    B[i][j] = 1.0 + i + j;

  for (i=0; i<row_A; i++)
  for (j=0; j<col_B; j++)
    C[i][j] = 2.0;
                
  transpose_matrix(B, row_B, col_B, Bt);;

  printf("\nA:\n");
  print_matrix((double**)A, row_A, col_A);
  printf("\nB:\n");
  print_matrix((double**)B, row_B, col_B);
  printf("\nC:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");

  multiply_matrix_v((double**)A, row_A, col_A, (double**)Bt, col_B, (double**)C);
  printf("\nC <- A * B:\n");
  print_matrix((double**)C, row_C, col_C);
  printf("\n");
}

void test_multiply_2() {
  int size = 17;
#ifdef ORDER
  size = ORDER;
#endif

  printf("Running blocked matrix multiply.\n");
  printf("  Matrix side length: %d\n", size);
  printf("  Block columns is:      %d\n", BLOCK_COLS);
  printf("  Block rows is:         %d\n", BLOCK_ROWS);

  int i,j,k;
  double wall_start, wall_end;
  double exp_start, exp_end;
  double e  = 0.0;
  double ee = 0.0;
  // double a_val = 3.0;
  // double b_val = 4.0;
  // double v  = a_val * b_val * size;
  double** A;    
  double** B;    
  double** B_T;    
  double** C;
  double** V; // used for validation

  int validate = 0;
  // validate = 1;

  printf("Allocating and filling matrices...\n"); fflush(stdout);
  srand((unsigned int)time(NULL));

  C = (double**)aligned_alloc(64,size*sizeof(double*));
  for (i=0; i<size; i++) C[i] = (double*)aligned_alloc(64,size*sizeof(double));
  A = (double**)aligned_alloc(64,size*sizeof(double*));
  for (i=0; i<size; i++) A[i] = (double*)aligned_alloc(64,size*sizeof(double));
  B = (double**)aligned_alloc(64,size*sizeof(double*));
  for (i=0; i<size; i++) B[i] = (double*)aligned_alloc(64,size*sizeof(double));
  B_T = (double**)aligned_alloc(64,size*sizeof(double*));
  for (i=0; i<size; i++) B_T[i] = (double*)aligned_alloc(64,size*sizeof(double));
  C = (double**)aligned_alloc(64,size*sizeof(double*));
  for (i=0; i<size; i++) C[i] = (double*)aligned_alloc(64,size*sizeof(double));
  V = (double**)aligned_alloc(64,size*sizeof(double*));
  for (i=0; i<size; i++) V[i] = (double*)aligned_alloc(64,size*sizeof(double));

  #pragma omp parallel for private(i,j,k)
  for (i=0; i<size; i++) {
  for (j=0; j<size; j++) {
    A[i][j] = (double)rand()/(double)(RAND_MAX);
    B[i][j] = (double)rand()/(double)(RAND_MAX);
    C[i][j] = 0.0;
    V[i][j] = 0.0;
  }
  }
  transpose_matrix(B, size, size, B_T);

  //fill validation matrix based on naive implementation
  if (validate) { 
    printf("producing validation matrix...\n"); fflush(stdout);
    multiply_matrix_vp(A, size, size, B_T, size, V);
  }

  // printf("  sizeof A is:        %d\n", sizeof(A[0]));

  printf("Performing multiplication experiment...\n"); fflush(stdout);
  wall_start = omp_get_wtime();
  // printf("Using default sequential implementation...\n"); fflush(stdout);
  // multiply_matrix_s(A, size, size, B,   size, C);

#ifdef DEFAULT
  printf("Using default parallel implementation..."); fflush(stdout);
  exp_start = omp_get_wtime();
  multiply_matrix_d(A, size, size, B,   size, C);
  exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));
#endif

#ifdef INTERCHANGE
  printf("Using loop interchanged parallel implementation..."); fflush(stdout);
  exp_start = omp_get_wtime();
  multiply_matrix_i(A, size, size, B,   size, C);
  exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));
#endif

#ifdef TRANSPOSE
  printf("Using transposed parallel implementation..."); fflush(stdout);
  exp_start = omp_get_wtime();
  multiply_matrix_t(A, size, size, B_T, size, C);
  exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));
#endif

#ifdef UNROLLJAM
  printf("Using unroll-jam parallel implementation..."); fflush(stdout);
  exp_start = omp_get_wtime();
  multiply_matrix_uj(A, size, size, B_T, size, C);
  exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));
#endif

#ifdef BLOCKED
  printf("Using blocked parallel implementation..."); fflush(stdout);
  exp_start = omp_get_wtime();
  multiply_matrix_b(A, size, size, B_T, size, C);
  exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));
#endif

#ifdef BLOCK_UNROLL
  printf("Using blocked unrolled implementation..."); fflush(stdout);
  exp_start = omp_get_wtime();
  multiply_matrix_bu(A, size, size, B_T, size, C);
  exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));
#endif

  // printf("Using blocked interchange parallel implementation..."); fflush(stdout);
  // exp_start = omp_get_wtime();
  // multiply_matrix_bi(A, size, size, B_T, size, C);
  // exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));

  // printf("Using flipped, blocked parallel implementation...\n"); fflush(stdout);
  // exp_start = omp_get_wtime();
  // multiply_matrix_f(A, size, size, B_T, size, C);
  // exp_end = omp_get_wtime(); printf(" %fs\n", (exp_end - exp_start));
  wall_end = omp_get_wtime();


  if (validate) { 
    printf("Checking result...\n"); fflush(stdout);
    // #pragma omp parallel for
    // #pragma omp parallel for reduction(+:ee) private(i,j)
    for (i=0; i<size; i++) {
      for (j=0; j<size; j++) {
        ee += (C[i][j] - V[i][j])*(C[i][j] - V[i][j]);
      }
    }
    if (ee > 0.05) {
      printf("Multiply complete: Falied\n");
      printf("Error:    %f\n", ee);
    } else {
      printf("Multiply complete: Success\n");
      printf("Time: %fs\n", (wall_end - wall_start));
      printf("FLOPS Theoretical: %f\n", ((double)size*(double)size*(double)size*2.0));
      printf("FLOPS per second:  %f\n", ((double)size*(double)size*(double)size*2.0/(wall_end - wall_start)));
    }
  } else {
    printf("Multiply complete, no validation.\n");
    printf("Time: %fs\n", (wall_end - wall_start));
    printf("FLOPS Theoretical: %f\n", ((double)size*(double)size*(double)size*2.0));
    printf("FLOPS per second:  %f\n", ((double)size*(double)size*(double)size*2.0/(wall_end - wall_start)));
  }


  for (i=0; i<size; i++) {
    free(A[i]);
    free(B[i]);
    free(C[i]);
    free(V[i]);
  }
  free(A);
  free(B);
  free(C);
  free(V);
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

/*
 * Miss Count micro benchmark
 *
 * 
 *
 *    
 *
 * A and B are both square matrix. They are statically allocated and
 * initialized with constant number, so we can focus on the parallelism.
 *
 * usage: mpirun -np <N> mm_mpi [-t]
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>


#ifdef USE_CALI
#include <caliper/cali.h>
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

#define CACHE_L1 1
#define CACHE_L2 2
#define CACHE_L3 3



struct Inputs {
   char  cache_level;
   int   threads;
};


void test_L1();

void get_input(int argc, char **argv, struct Inputs* input);
void matrix_init(TYPE** A, TYPE** B, TYPE** C, size_t row_len);
void transpose_mat(TYPE* A);
void matrix_free(TYPE* A, TYPE* B, TYPE* C, size_t size);
void print_mat(TYPE* C);



// main function
int main(int argc, char **argv) {
	
  size_t i,j,k,r;

  double run_time, start, end;

  struct Inputs input;
  get_input(argc, argv, &input);
  omp_set_num_threads(input.threads);

  start = omp_get_wtime();

#ifdef USE_CALI
CALI_MARK_BEGIN("full");
#endif

#pragma omp parallel
{
  test_L1();
}

#ifdef USE_CALI
CALI_MARK_END("full");
#endif

  end = omp_get_wtime();

  printf("Run time: %f\n", end - start);
  
  return 0;
}


// the square matrices are ORDERxORDER
// For Skyake the L1 cache is 32k = 4000 doubles
// 63*63 = 3969 doubles 
void test_L1() {

  int order = 2000;
  // int order = 63;
  size_t i,j,k,r;

  TYPE *A, *B, *C;
  matrix_init(&A, &B, &C, order);


  for(i = 0; i < order; i++){
    for(j = 0; j < order; j++){
      for(k = 0; k < order; k++) {
        C[i*order+j] += A[i*order+k] * B[j*order+k];
      }
    }
  }

  matrix_free(A,B,C,order);

}


/*************************************************************\

                      Utility Functions

\*************************************************************/


void get_input(int argc, char **argv, struct Inputs* input) {

  int i = 1;

  input->cache_level = CACHE_L1;
  input->threads = 4;

  for(i = 1; i < argc; i++) {

    if ( !(strcmp("-1", argv[i])) || !(strcmp("--cache_l1", argv[i])) )
      input->cache_level = CACHE_L1;

    else if ( !(strcmp("-2", argv[i])) || !(strcmp("--cache_l2", argv[i])) )
      input->cache_level = CACHE_L2;

    else if ( !(strcmp("-3", argv[i])) || !(strcmp("--cache_l3", argv[i])) )
      input->cache_level = CACHE_L3;

    if ( !(strcmp("-t", argv[i])) || !(strcmp("--threads", argv[i])) ) {
      if (i++ < argc){
        input->threads = atoi(argv[i]);
      } else {
        printf("Please include a thread count that option\n");
        exit(1);
      }

    }

  }



}


// Initialize the matrices (uniform values to make an easier check)
void matrix_init(TYPE** A, TYPE** B, TYPE** C, size_t row_len) {
  size_t i, j;

  (*A) = (TYPE*)malloc(row_len*row_len*sizeof(TYPE));
  (*B) = (TYPE*)malloc(row_len*row_len*sizeof(TYPE));
  (*C) = (TYPE*)malloc(row_len*row_len*sizeof(TYPE));

  if( ((*A) == NULL) || ((*B) == NULL) || ((*C) == NULL) ) {
    printf("ERROR allocating memory\n");
    exit(1);
  }

  for (j=0; j<row_len*row_len; j++) {
    (*A)[j] = AVAL;
    (*B)[j] = BVAL;
    (*C)[j] = 0.0;
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

  free(A);
  free(B);
  free(C);

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



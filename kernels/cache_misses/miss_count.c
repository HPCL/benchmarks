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
void transpose_mat(TYPE* A, int order);
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
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
#pragma omp parallel
{
cali_set_int(thread_attr, omp_get_thread_num());
}
#endif

#pragma omp parallel
{
  test_L1();
}


  end = omp_get_wtime();

  printf("Run time: %f\n", end - start);
  
  return 0;
}


// the square matrices are ORDERxORDER
// For Skyake the L1 cache is 32k = 4000 doubles
//    - so I'm assuming it is divisible by the cache line
//    - 64 bytes = 8 doubles
// 64*64 = 4096 doubles 
void test_L1() {

  // int order = 2000;
  int order = 64;
  int exp_count = 0;
  size_t i,j,k,r;

  int cache_line_order[] = {6,1,5,2,0,7,3,4};

  TYPE *A, *B, *C;
  matrix_init(&A, &B, &C, order);

  for (exp_count = 0; exp_count < 100; exp_count++) {
    #ifdef USE_CALI
    CALI_MARK_BEGIN("cache_prep");
    #endif

      // Load all matrices into the L2 cache
      // fill L1 cache with the C matrix
      transpose_mat(A, order);
      transpose_mat(B, order);
      transpose_mat(C, order);

    #ifdef USE_CALI
    CALI_MARK_END("cache_prep");
    #endif

    #ifdef USE_CALI
    CALI_MARK_BEGIN("cache_test");
    #endif
      for (int k = 0; k < 8; k++) {         // choose element in the cache line
        for (int i = 0; i < order; i+=8) {  // choose the row
          for (int j = 0; j < 8; j++) {     // choose the  cache line column

            int index_1 = i*order + cache_line_order[j]*8 + k; 
            int index_2 = i*order + cache_line_order[j]*8 + 3;
            A[index_1] = A[index_1] + A[index_2];

          }
        }
      }
    #ifdef USE_CALI
    CALI_MARK_END("cache_test");
    #endif
  }
  // for(i = 0; i < order; i++){
  //   for(j = 0; j < order; j++){
  //     for(k = 0; k < order; k++) {
  //       C[i*order+j] += A[i*order+k] * B[j*order+k];
  //     }
  //   }
  // }

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

void transpose_mat(TYPE* A, int order) {

  TYPE temp;
  for (int i=1; i<order; i++) {
    for (int j=i; j<order; j++) {
      temp = A[i*order+j];
      A[i*order+j]=A[j*order+i];
      A[j*order+i]=temp;
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



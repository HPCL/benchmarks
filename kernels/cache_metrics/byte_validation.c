/*
 * Benchmark to test the measurement of bytes requested by the CPU
 *
 * 
 *
 *    
 *
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>


#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#ifndef SIZE
#define SIZE 65536   // 4x doubles in Intel L1
#endif

#ifndef NUM_OPS
#define NUM_OPS 2147483616 // 2^31
#endif

#define AVAL  7.0    // initial value of A
#define BVAL  5.0    // initial value of B
#define TOL   0.001  // tolerance used to check the result

#define TYPE double

#define TRUE  1
#define FALSE 0

struct Inputs {
   int   simd;
   int   r_size;
};


void scalar_simd_loop(TYPE* A, TYPE* B, TYPE* C, size_t size, size_t simd_frac, size_t num_loops);
void reduction_loop(TYPE* A, TYPE* B, TYPE* C, size_t size, size_t r_size, size_t num_loops);
void get_input(int argc, char **argv, struct Inputs* input);
void vector_init(TYPE** A, TYPE** B, TYPE** C, size_t row_len);
void vector_free(TYPE* A, TYPE* B, TYPE* C, size_t size);
void print_mat(TYPE* C);



// main function
int main(int argc, char **argv) {
	
  size_t i,j,k,r;
  size_t size,num_loops;

  double run_time, t1, t2, t3;

  struct Inputs input;
  get_input(argc, argv, &input);

#ifdef USE_CALI
  cali_init();
  cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
  cali_set_int(thread_attr, omp_get_thread_num());
#endif

  TYPE *A, *B, *C;

  size = SIZE;
  num_loops = NUM_OPS / (2*size);
  vector_init(&A, &B, &C, size);  
  // warm up cache
  #pragma omp simd
  for (int j = 0; j < size; j ++) {
    A[j] = 0.99*A[j];
    B[j] = 0.99*B[j];
    C[j] = 0.99*C[j];
  }
  printf("Settings:\n");
  printf("  Size:      %d\n", size);
  printf("  SIMD:      %d\n", input.simd);
  printf("  Num Ops:   %d\n", NUM_OPS);
  printf("  Num Loops: %d\n", num_loops);
  
  t1 = omp_get_wtime();
  scalar_simd_loop(A, B, C, size, input.simd, num_loops);
  t2 = omp_get_wtime();
  reduction_loop(A, B, C, size, input.r_size, num_loops);
  t3 = omp_get_wtime();

  vector_free(A,B,C,size);
  printf("Scalar-SIMD time: %f\n", t2 - t1);
  printf("Reduction time:   %f\n", t3 - t2);

  return 0;
}


void scalar_simd_loop(TYPE* A, TYPE* B, TYPE* C, size_t size, 
                    size_t simd_frac, 
                    size_t num_loops) {
  TYPE scale = 0.99; 
  if (simd_frac < 1) simd_frac = 1;
  if (size < simd_frac) simd_frac = size;
  // warm up cache

#ifdef USE_CALI
CALI_MARK_BEGIN("simd_loop");
#endif

  for (int i = 0; i < num_loops; i++) {

    int j = 0;
    #pragma omp simd
    for (j; j < simd_frac; j++) {
      C[j] = A[j]+scale*B[j];
    }

    for (j; j < size; j++) {
      B[j] = A[j]-scale*B[j-1];
    }


  }

#ifdef USE_CALI
CALI_MARK_END("simd_loop");
#endif

}


void reduction_loop(TYPE* A, TYPE* B, TYPE* C, size_t size, 
                    size_t r_size, 
                    size_t num_loops) {

  TYPE scale = 0.99; 
  if (r_size < 1) r_size = 1;
  if (size < r_size) r_size = size;

#ifdef USE_CALI
CALI_MARK_BEGIN("reduct_loop");
#endif

  for (int i = 0; i < num_loops*2; i++) {

    int j = 0;
    int k = 0;
    for (j = 0; j < size; j+=r_size) {
      double temp = C[j];
      #pragma omp simd reduction(+:temp)
      for (k = j; k < j+r_size; k++) {
        temp += A[k]+scale*B[k];
      }
      C[j] = temp;
    }

  }

#ifdef USE_CALI
CALI_MARK_END("reduct_loop");
#endif

}


/*************************************************************\

                      Utility Functions

\*************************************************************/


void get_input(int argc, char **argv, struct Inputs* input) {

  int i = 1;

  input->simd   = SIZE/2;
  input->r_size = 16;

  for(i = 1; i < argc; i++) {


    if ( !(strcmp("-s", argv[i])) || !(strcmp("--simd_fraction", argv[i])) ) {
      if (i++ < argc){
        input->simd = atoi(argv[i]);
      } else {
        printf("Please include a number of simd ops (less than SIZE) with that option\n");
        exit(1);
      }

    }

    if ( !(strcmp("-r", argv[i])) || !(strcmp("--reduction_size", argv[i])) ) {
      if (i++ < argc){
        input->simd = atoi(argv[i]);
      } else {
        printf("Please include a number of simd ops (less than SIZE) with that option\n");
        exit(1);
      }

    }

  }



}


// Initialize the vectors (uniform values to make an easier check)
void vector_init(TYPE** A, TYPE** B, TYPE** C, size_t row_len) {
  size_t i, j;

  if( ((row_len) % 64) != 0 ) {
    printf("ERROR aligning memory; make sure size is multiple of 64 bytes.\n");
    exit(1);
  }

  (*A) = (TYPE*)aligned_alloc(64, row_len*sizeof(TYPE));
  (*B) = (TYPE*)aligned_alloc(64, row_len*sizeof(TYPE));
  (*C) = (TYPE*)aligned_alloc(64, row_len*sizeof(TYPE));

  if( ((*A) == NULL) || ((*B) == NULL) || ((*C) == NULL) ) {
    printf("ERROR allocating memory\n");
    exit(1);
  }

  for (j=0; j<row_len; j++) {
    (*A)[j] = AVAL;
    (*B)[j] = BVAL;
    (*C)[j] = 0.0;
  }
 
}

void vector_free(TYPE* A, TYPE* B, TYPE* C, size_t size) {

  free(A);
  free(B);
  free(C);

}


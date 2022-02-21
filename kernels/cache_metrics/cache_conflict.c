/*
 * Benchmark to force severe cache conflicts
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
#define SIZE 16384   // 2x doubles in A64FX L1
#endif

#ifndef NUM_OPS
#define NUM_OPS 268435456   
#endif

#define AVAL  3.0    // initial value of A
#define BVAL  5.0    // initial value of B
#define TOL   0.001  // tolerance used to check the result

#define TYPE double

#define TRUE  1
#define FALSE 0

struct Inputs {
   int   threads;
   int   offset;
};


void loop_over_data(TYPE* A, size_t size, size_t offset, size_t num_loops);
void get_input(int argc, char **argv, struct Inputs* input);
void vector_init(TYPE** A, TYPE** B, TYPE** C, size_t row_len);
void vector_free(TYPE* A, TYPE* B, TYPE* C, size_t size);
void print_mat(TYPE* C);



// main function
int main(int argc, char **argv) {
	
  size_t i,j,k,r;
  size_t size,offset,num_loops;

  double run_time, start, end;

  struct Inputs input;
  get_input(argc, argv, &input);
  omp_set_num_threads(input.threads);


#ifdef USE_CALI
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
#pragma omp parallel
{
cali_set_int(thread_attr, omp_get_thread_num());
}
#endif

  TYPE *A, *B, *C;

  size = SIZE;
  offset = input.offset;
  num_loops = NUM_OPS * offset / size;
  vector_init(&A, &B, &C, size);
  printf("Settings:\n");
  printf("  Size:      %d\n", size);
  printf("  Num Ops:   %d\n", NUM_OPS);
  printf("  Offset:    %d\n", offset);
  printf("  Num Loops: %d\n", num_loops);
  
  start = omp_get_wtime();
  loop_over_data(A, size, offset, num_loops);
  end = omp_get_wtime();

  vector_free(A,B,C,size);
  printf("Run time: %f\n", end - start);

  return 0;
}


void loop_over_data(TYPE* A, size_t size, size_t offset, size_t num_loops) {

  TYPE scale = 1.012;

  for (int i = 0; i < num_loops; i++) {
    for (int j = 0; j < size; j += offset) {
      A[j] = scale*A[j];
    }
  }

}


/*************************************************************\

                      Utility Functions

\*************************************************************/


void get_input(int argc, char **argv, struct Inputs* input) {

  int i = 1;

  input->offset  = 16;
  input->threads = 4;

  for(i = 1; i < argc; i++) {


    if ( !(strcmp("-t", argv[i])) || !(strcmp("--threads", argv[i])) ) {
      if (i++ < argc){
        input->threads = atoi(argv[i]);
      } else {
        printf("Please include a thread count that option\n");
        exit(1);
      }

    }

    if ( !(strcmp("-o", argv[i])) || !(strcmp("--offset", argv[i])) ) {
      if (i++ < argc){
        input->offset = atoi(argv[i]);
      } else {
        printf("Please include an integer that option\n");
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


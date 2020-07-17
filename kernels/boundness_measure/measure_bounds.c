/*
 * micro benchmark to vary the mmemory vs core boundness
 *
 * author Brian J Gravelle
 * gravelle @cs.uoregon.edu or @lanl.gov
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
   char    cache_level; // which cache level to focus on
   size_t  threads;     // number of threads to simultaneously execute the benchmark
   size_t  flops;       // number of flops per iteration (divide by 8 for arithmetic intensity)
};


void test_boundness(struct Inputs* input);

void get_input(int argc, char **argv, struct Inputs* input);
void data_init(TYPE** A, TYPE** B, TYPE** C, size_t size);
void fill_cache(TYPE* A, size_t size);
void matrix_free(TYPE* A, TYPE* B, TYPE* C, size_t size);
void print_mat(TYPE* C);



// main function
int main(int argc, char **argv) {
	
  size_t i,j,k,r;

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

  start = omp_get_wtime();

  #pragma omp parallel
  {
  test_boundness(&input);
  }

  end = omp_get_wtime();

  printf("Run time: %f\n", end - start);
  
  return 0;
}


void test_boundness(struct Inputs* input) {

  size_t L1_size = 4096;        // one L1 cache (number of doubles)
  size_t L2_size = 128000;      // one L2 cache (number of doubles)
  size_t size = 128;
  size_t batch_size = 64;

  srand((unsigned int)omp_get_wtime());

  if(input->cache_level == CACHE_L1){
    size = L1_size*70;     // a few caches worth so the blocked run takes time
    batch_size = L1_size;
  } else if(input->cache_level == CACHE_L2) {
    size = L2_size*10;     // a few caches worth so the blocked run takes time
    batch_size = L2_size;
  }


  size_t flops = input->flops;  // divide by 8 for double to get arithmetic intensity

  size_t exp_count = 0;
  size_t i,j,k,r;

  size_t data_order[batch_size];
  for (size_t i = 0; i < batch_size; i++) {
    data_order[i] = rand()%batch_size;
  }

  TYPE *A, *B, *C;
  data_init(&A, &B, &C, size);

  // Load A array into the lower caches
  fill_cache(A, size);

  for (exp_count = 0; exp_count < 100; exp_count++) {
    #ifdef USE_CALI
    CALI_MARK_BEGIN("cache_prep");
    #endif

    // Fill L2 and L1 with C matrix that won't be used
    fill_cache(C, size);

    #ifdef USE_CALI
    CALI_MARK_END("cache_prep");
    #endif

    #ifdef USE_CALI
    CALI_MARK_BEGIN("cache_test");
    #endif
    for (size_t b = 0; b < size; b+=batch_size) {
      for (size_t i = 0; i < batch_size; i++) {
        int index = b+data_order[i];
        TYPE sum = A[index];
        for(j = 1; j < flops; j++) {
          sum += A[index];
        }
        A[index] += sum;
      }
    }
    #ifdef USE_CALI
    CALI_MARK_END("cache_test");
    #endif
  }

  matrix_free(A,B,C,size);

}


/*************************************************************\

                      Utility Functions

\*************************************************************/


void get_input(int argc, char **argv, struct Inputs* input) {

  int i = 1;

  input->cache_level = CACHE_L1;
  input->threads     = 4;
  input->flops       = 8;

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
        printf("Please include a thread count with that option\n");
        exit(1);
      }

    }

    if ( !(strcmp("-f", argv[i])) || !(strcmp("--flops", argv[i])) ) {
      if (i++ < argc){
        input->flops = atoi(argv[i]);
      } else {
        printf("Please include a flop count with that option\n");
        exit(1);
      }

    }

  }

}


// Initialize the matrices (uniform values to make an easier check)
void data_init(TYPE** A, TYPE** B, TYPE** C, size_t size) {
  size_t i, j;


  if( (size % 64) != 0 ) {
    printf("ERROR aligning memory; make sure size is multiple of 64 bytes.\n");
    exit(1);
  }

  (*A) = (TYPE*)aligned_alloc(64, size*sizeof(TYPE));
  (*B) = (TYPE*)aligned_alloc(64, size*sizeof(TYPE));
  (*C) = (TYPE*)aligned_alloc(64, size*sizeof(TYPE));

  if( ((*A) == NULL) || ((*B) == NULL) || ((*C) == NULL) ) {
    printf("ERROR allocating memory\n");
    exit(1);
  }

  for (j=0; j<size; j++) {
    (*A)[j] = AVAL;
    (*B)[j] = BVAL;
    (*C)[j] = 0.0;
  }
 
}

void fill_cache(TYPE* A, size_t size) {

  // random order from online die roller
  int cache_line_order[] = {6,1,5,2,0,7,3,4};
  int k = 0;
  for (int j=0; j<8; j++) {
    for (int i=size-8; i>=0; i-=8) {
      int index_1 = i+cache_line_order[j];
      int index_2 = (k+cache_line_order[j])%size;
      double temp = A[index_2];
      A[index_1] += temp;
      k += 8;
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



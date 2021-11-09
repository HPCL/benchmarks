#ifndef  __FP_CRUNCH_H__
#define __FP_CRUNCH_H__


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#ifdef DP
#define DATA_TYPE double
#define DATA_TYPE_NAME "double"
#else
#define DATA_TYPE float
#define DATA_TYPE_NAME "single"
#endif

#ifndef DATA_SIZE
#define DATA_SIZE 1024
#endif 

void fma(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c
  );

DATA_TYPE fma_v(int n_trials);

inline void add(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c
  );

inline void mul(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c
  );

#endif
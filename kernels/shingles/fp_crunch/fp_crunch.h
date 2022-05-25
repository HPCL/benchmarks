#ifndef  __FP_CRUNCH_H__
#define __FP_CRUNCH_H__


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
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

#define TRUE  1
#define FALSE 0

#ifndef DATA_SIZE
#define DATA_SIZE 1024
#endif 

#ifdef USE_COS
#define OP(x,y)    cos(x)
#elif USE_LOG
#define OP(x,y)    log(x)
#elif USE_DIV
#define OP(x,y)    x/y
#elif USE_ADD
#define OP(x,y)    x+y
#else
#define OP(x,y)    x * y
#endif

void my_fma(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c, 
  const int n_trials
  );

DATA_TYPE fma_v(int n_trials);

void add(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c, 
  const int n_trials
  );

void mul(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c, 
  const int n_trials
  );

#endif



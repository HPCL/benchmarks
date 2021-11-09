#include "fp_crunch.h"
 
// element-wise multiply add of three arrays
// 
void fma(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c
  ) {

  int i;

  // #pragma unroll(DATA_SIZE)
  #pragma omp simd
  for (i = 0; i < DATA_SIZE; i++) {
    mat_a[i] += mat_c[i] * mat_b[i];
  }

}

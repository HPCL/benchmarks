#include "fp_crunch.h"
 
// element-wise multiply add of three arrays
// 
void mul(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c,
  int size) {

  int i;
  #pragma openmp simd
  for (i = 0; i < rows; i++) {
    mat_c[i] = mat_a[i] * mat_b[i];
  }

}

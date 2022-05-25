#include "fp_crunch.h"
 

void init_arrays(
  DATA_TYPE* restrict fa, 
  DATA_TYPE* restrict fb, 
  DATA_TYPE* restrict fc
  ) {

  int i;

  for( int l = 0; l < DATA_SIZE; l++ )
  {
    fa[l] = 0.0001 + 0.0001*(double)l;
    fb[l] = 0.0001 + 0.0001*(double)l;
    fc[l] = 0.0001 + 0.0001*(double)l;
  }

}


// element-wise multiply add of three arrays
// 
DATA_TYPE fma_v(int n_trials) {

  DATA_TYPE fa[ DATA_SIZE ];
  DATA_TYPE fb[ DATA_SIZE ];
  DATA_TYPE fc[ DATA_SIZE ];
  DATA_TYPE norm = 0.;

  init_arrays(fa, fb, fc); 

  // #pragma unroll(DATA_SIZE)
  for( long t = 0; t < n_trials; t++ ){
    #pragma omp parallel for
    for (int i = 0; i < DATA_SIZE; i++) {
      fa[i] += fc[i] * fb[i];
    }
  }

  // norm for validation
  #pragma omp parallel for shared(fa) reduction(+:norm)
  for (int i = 0; i < DATA_SIZE; i++) {
    norm += fa[i] * fa[i];
  }

  return norm;
}

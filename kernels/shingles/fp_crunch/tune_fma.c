#include "fp_crunch.h"
 

// element-wise multiply add of three arrays
// 
void fma(
  DATA_TYPE* restrict mat_a, 
  DATA_TYPE* restrict mat_b, 
  DATA_TYPE* restrict mat_c
  ) {

/*@ begin PerfTuning (
 def build {
   arg build_command = 'icc';
   #arg libs = '-lrt';  # Only needed on linux
 } 
 def performance_counter {
   arg repetitions = 5;
 }
 def performance_params {  
   param VEC[] = [False,True];
   param UF[] = range(1,64);
   param CFLAGS[] = ['-O0', '-O1', '-O2', '-O3'];
   constraint divisible_by_eight = (UF % 8 == 0);
 }
 def input_params {
   param N[] = [1024];
   param T[] = [52428800];
 }
 def input_vars {
   decl static double mat_a[N] = random;
   decl static double mat_b[N] = random;
   decl static double mat_c[N] = random;
 }
 def search {
   arg algorithm = 'Exhaustive';
 }
) @*/

  int n=N;
  int t=T;
  register int i;
  register int j;

/*@ begin Loop ( 
  transform Composite(
    unrolljam = (['i'],[UF]),
    vector = (VEC, ['omp simd'])
   )
  for (j=0; j<=t-1; j=j+1)
    for (i=0; i<=n-1; i=i+1)
      mat_a[i] += mat_c[i] * mat_b[i];

) @*/

  for (j=0; j<=t-1; j=j+1)
    for (i=0; i<=n-1; i=i+1)
      mat_a[i] += mat_c[i] * mat_b[i];


/*@ end @*/
/*@ end @*/


}

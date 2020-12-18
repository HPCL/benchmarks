
//multiply matrices together
//pre all matrices are initialized, c shouldn't have any important data in it
//     mat b is trnasposed
//     rows in b == cols in a
//     c is initialized to the same size as b
//post mat_c has the result of multipling mat_a and mat_b
void multiply_matrix_t(double** restrict mat_a, int rows_a, int cols_a, 
                       double** restrict mat_b, int cols_b, 
                       double** restrict mat_c) {


  /*@ begin PerfTuning (
    def build {
      arg build_command = 'gcc-9 -fopenmp -O3 -mcmodel=large ';
      #arg libs = '-lrt';  # Only needed on linux
    } 
    def performance_counter {
      arg repetitions = 5;
    }
    def performance_params {  

      #param T1_I[] = [1,16,32,64,128,256,512];
      #param T1_J[] = [1,16,32,64,128,256,512];
      param T1_J[] = [1,32,64,128,512,1024];

      # param U_J[] = [1,4,8,16];
      # param U_K[] = [1,8,16];
      param OMP[] = [True];
      
      # constraint unroll_limit = ((U_J == 1) or (U_K == 1));
    }
    def input_params {
      let N = [8192, 16384];
      param rows_a[] = N;
      param cols_a[] = N;
      param cols_b[] = N;
    }
    def input_vars {
    decl static double A[rows_a][cols_a] = random;
    decl static double B[cols_b][cols_a] = random;
    decl static double C[rows_a][cols_b] = random;
    }
    def search {
      arg algorithm = 'Exhaustive';
    }
  ) @*/

  int i, j, k;
  int ii, jj;
  int iii, jjj;

#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))

// b is transposed

/*@ begin Loop(
  transform Composite(
    tile = [('j',T1_J,'jj')],
    openmp = (OMP, 'omp parallel for private(iii,jjj,ii,jj,i,j,k)')
  )
  for(i=0; i<=rows_a-1; i++) 
    for(j=0; j<=cols_b-1; j++)   
      for(k=0; k<=cols_a-1; k++) 
        C[i][j] = C[i][j] + A[i][k] * B[j][k]; 
) @*/

/*@ end @*/
/*@ end @*/


}

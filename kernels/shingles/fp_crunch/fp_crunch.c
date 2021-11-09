
#include "fp_crunch.h"

#ifdef USE_CALI
#include <caliper/cali.h>
#endif


struct Inputs 
{
  int flops;   // number of flops to compute
  int threads; // threads to run (each gets its own data)
  int multiply; // threads to run (each gets its own data)
};
void get_input(int argc, char **argv, struct Inputs* input);

int main(int argc, char **argv) {



  //----------------------------------------------------------------------------//
  // Get input and set parameters
  //----------------------------------------------------------------------------//
  struct Inputs input;
  DATA_TYPE norm;
  int n_trials;
  int n_flops;
  int n_threads;
  int flops_per_elm = 2; // for fma
  get_input(argc, argv, &input);
  omp_set_num_threads(input.threads);

  n_threads = input.threads;
  n_flops   = input.flops;
  n_trials  = n_flops / (DATA_SIZE*flops_per_elm);
  n_trials  = n_trials * input.multiply;


  //----------------------------------------------------------------------------//
  // Print initial output.
  //----------------------------------------------------------------------------//

  // printf( "%30s: %d\n%30s: %s\n%30s: %s\n%30s: %d\n%30s: %d\n%30s: %ld\n%30s: ",
  printf( "%30s: %d\n%30s: %s\n%30s: %d\n%30s: %d\n%30s: %f\n%30s: %d\n%30s: %d\n%30s: ",
          "Threads", omp_get_max_threads(),
          "Data type", DATA_TYPE_NAME,
          "Array size", DATA_SIZE,
          "Flops/thrd (before mult)", n_flops,
          "GFlops per thread", 1.0e-9 * n_flops,
          "Number of Trials", n_trials,
          "Trial multiplier", input.multiply,
          "Time" );

  fflush( stdout );

  //----------------------------------------------------------------------------//
  // Warm up the threads.
  //----------------------------------------------------------------------------//

  #pragma omp parallel
  {}

  //----------------------------------------------------------------------------//
  // Initialize Caliper.
  //----------------------------------------------------------------------------//

   #ifdef USE_CALI
    cali_init();
    cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);

  #ifdef USE_CALI_REG
    #pragma omp parallel
    {
    cali_set_int(thread_attr, omp_get_thread_num());
    }
  #endif
  #ifdef USE_CALI_UNCORE
    cali_set_int(thread_attr, omp_get_thread_num());
  #endif

  #endif

  //----------------------------------------------------------------------------//
  // Begin calculation.
  //----------------------------------------------------------------------------//

  double t0;
  double t1;
  #pragma omp parallel
  {
    DATA_TYPE fa[ DATA_SIZE ];
    DATA_TYPE fb[ DATA_SIZE ];
    DATA_TYPE fc[ DATA_SIZE ];

    // Initialization.
    for( int l = 0; l < DATA_SIZE; l++ )
    {
      fa[l] = 0.0001 + 0.0001*(double)l;
      fb[l] = 0.0001 + 0.0001*(double)l;
      fc[l] = 0.0001 + 0.0001*(double)l;
    }

    #pragma omp barrier
    if ( omp_get_thread_num() == 0 ) {
      t0 = omp_get_wtime();
      #ifdef USE_CALI_UNCORE
      CALI_MARK_BEGIN( "fma_crunch" );
      #endif
    }
    #ifdef USE_CALI_REG
    CALI_MARK_BEGIN( "fma_crunch" );
    #endif
    #pragma omp barrier

    int i;
    // Perform multiple trials for timing.
    // #pragma unroll(4096)
    for( long t = 0; t < n_trials; t++ )
    {
      // Crunch an FMA.
      fma(fa, fb, fc);

      // #pragma unroll(DATA_SIZE)
      // for (i = 0; i < DATA_SIZE; i++) {
      //   fc[i] += fa[i] * fb[i];
      // }
    }


    #pragma omp barrier
    #ifdef USE_CALI_REG
    CALI_MARK_END( "fma_crunch" );
    #endif
    if ( omp_get_thread_num() == 0 ) {
      #ifdef USE_CALI_UNCORE
      CALI_MARK_BEGIN( "fma_crunch" );
      #endif
      t1 = omp_get_wtime();
    }

    // Prevent dead code elimination.
    if ( (fa[0] == 0.0) ||  (fb[0] == 0.0) || (fc[0] == 0.0 ) )
    {
      printf("You messed up\n\n");
      exit(1);
    }

    if ( omp_get_thread_num() == 0 ) {
      #pragma omp parallel for shared(fa) reduction(+:norm)
      for (int i = 0; i < DATA_SIZE; i++) {
        norm += fa[i] * fa[i];
      }
    }

    // Bigger hammer to prevent dead code elimination.
    // FILE *f = fopen( "/dev/null", "r+" );
    // fwrite( fa, sizeof(fa), 1, f );
    // fclose( f );

  } // parallel


  //----------------------------------------------------------------------------//
  // Compute and print final output.
  //----------------------------------------------------------------------------//

  // const double flops = (double) n_threads *
  //                      (double) n_flops *
  //                      (double) input.multiply;

  // const double gflops = 1.0e-9 *
  //                       (double) n_threads *
  //                       (double) n_flops *
  //                       (double) input.multiply;

  const double flops = (double) n_threads *
                       (double) n_trials *
                       (double) flops_per_elm *
                       (double) DATA_SIZE;

  const double gflops = 1.0e-9 *
                        (double) n_threads *
                        (double) n_trials *
                        (double) flops_per_elm *
                        (double) DATA_SIZE;

  printf( "%.6f s\n%30s: %.1f GFLOPs\n%30s: %.1f GFLOP/s\n%30s: %.5f \n%30s: %.5f \n\n", 
          t1 - t0, 
          "Flops",
          gflops, 
          "Performance",
          gflops / ( t1 - t0 ),
          "Benchmark Norm",
          norm,
          "Theoretical Norm",
          fma_v(n_trials) );


  //----------------------------------------------------------------------------//
  // Done.
  //----------------------------------------------------------------------------//

  return 0;
}



void get_input(int argc, char **argv, struct Inputs* input) {

  int i = 1;

  input->flops      = 65536; // 2^16
  input->threads    = 16;
  input->multiply   = 1;

  for(i = 1; i < argc; i++) {

    if ( !(strcmp("-h", argv[i])) || !(strcmp("--help", argv[i])) ) {
      printf("\n");
      printf("fp crunch problem help usage:\n");
      printf("  -h --help .................. print this message\n");
      printf("  -f --num_flops [] .......... set the number of flops per array (2^16)\n");
      printf("  -m --multiplier [] ......... set a multiplier for extra reps to increase runtime (1) \n");
      printf("  -t --num_threads [] ........ set the number of threads (16)\n");
      printf("\n");
      printf("Note that the size of the arrays (currently %d) can be set at compile time\n", DATA_SIZE);
      printf("  by using DATA_SIZE=[size] in the make command. The number of trials will\n");
      printf("  be set based on the number of flops and data size. These values are all set\n");
      printf("  on a per thread basis and data is allocated privately to each thread.\n");
      printf("\n");
      exit(0);
    }

    if ( !(strcmp("-f", argv[i])) || !(strcmp("--num_flops", argv[i])) ) {
      if (i++ < argc){
        input->flops = atoi(argv[i]);
      } else {
        printf("Please include a count of flops (ideally power of 2) with option f/num_flops\n");
        exit(1);
      }
    }

    if ( !(strcmp("-m", argv[i])) || !(strcmp("--multiplier", argv[i])) ) {
      if (i++ < argc){
        input->multiply = atoi(argv[i]);
      } else {
        printf("Please include an integer multiplierfor the reps with option m/multiplier\n");
        exit(1);
      }
    }

    if ( !(strcmp("-t", argv[i])) || !(strcmp("--num_threads", argv[i])) ) {
      if (i++ < argc){
        input->threads = atoi(argv[i]);
      } else {
        printf("Please include a number of threads with option t/num_threads\n");
        exit(1);
      }
    }

  }

}


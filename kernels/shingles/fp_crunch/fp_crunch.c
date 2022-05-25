
#include "fp_crunch.h"
#include <arm_sve.h>
#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))



struct Inputs 
{
  int  flops;   // number of flops to compute
  int  threads; // threads to run (each gets its own data)
  int  multiply; // threads to run (each gets its own data)
  char validate; // threads to run (each gets its own data)
};
void get_input(int argc, char **argv, struct Inputs* input);

int main(int argc, char **argv) {



  //----------------------------------------------------------------------------//
  // Get input and set parameters
  //----------------------------------------------------------------------------//
  struct Inputs input;
  int n_trials;
  int n_flops;
  int n_threads;
  char validate = FALSE;
  int flops_per_elm = 2; // for fma
  get_input(argc, argv, &input);
  omp_set_num_threads(input.threads);
  DATA_TYPE norm[input.threads];
  validate = input.validate;


  for( int l = 0; l < input.threads; l++ )
    norm[l] = 0.0;


  n_threads = input.threads;
  n_flops   = input.flops;
  n_trials  = n_flops / (DATA_SIZE*flops_per_elm);
  n_trials  = n_trials * input.multiply;

  // DATA_TYPE all_fa[n_threads][ DATA_SIZE ];
  // DATA_TYPE all_fb[n_threads][ DATA_SIZE ];
  // DATA_TYPE all_fc[n_threads][ DATA_SIZE ];


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
  #pragma omp parallel shared(norm)
  {

    int tid = omp_get_thread_num();
    // DATA_TYPE* fa = all_fa[omp_get_thread_num()];
    // DATA_TYPE* fb = all_fb[omp_get_thread_num()];
    // DATA_TYPE* fc = all_fc[omp_get_thread_num()];

    DATA_TYPE fa[ DATA_SIZE ] __attribute__ ((aligned (64)));
    DATA_TYPE fb[ DATA_SIZE ] __attribute__ ((aligned (64)));
    DATA_TYPE fc[ DATA_SIZE ] __attribute__ ((aligned (64)));

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

#ifdef UNROLL_INTR


  int64_t i = 0;
  int64_t n = DATA_SIZE;
  svbool_t pg = svwhilelt_b64(i, n);                    // [1]
  // do
  // {
  // #pragma omp simd
  // #pragma unroll
  for( i = 0; i < n; i += 4*svcntd() ) {
    svfloat64_t fa_vec   = svld1(pg, &fa[i]);
    svfloat64_t fb_vec   = svld1(pg, &fb[i]);
    svfloat64_t fc_vec   = svld1(pg, &fc[i]);
    
    svfloat64_t fa_vec_1 = svld1(pg, &fa[i+svcntd()]);
    svfloat64_t fb_vec_1 = svld1(pg, &fb[i+svcntd()]);
    svfloat64_t fc_vec_1 = svld1(pg, &fc[i+svcntd()]);
    
    svfloat64_t fa_vec_2 = svld1(pg, &fa[i+2*svcntd()]);
    svfloat64_t fb_vec_2 = svld1(pg, &fb[i+2*svcntd()]);
    svfloat64_t fc_vec_2 = svld1(pg, &fc[i+2*svcntd()]);
    
    svfloat64_t fa_vec_3 = svld1(pg, &fa[i+3*svcntd()]);
    svfloat64_t fb_vec_3 = svld1(pg, &fb[i+3*svcntd()]);
    svfloat64_t fc_vec_3 = svld1(pg, &fc[i+3*svcntd()]);
    // fa[i] += fc[i] * fb[i];
    for( long t = 0; t < n_trials; t++ ){
      fa_vec   = svmad_f64_x(pg, fb_vec,   fc_vec,   fa_vec);
      fa_vec_1 = svmad_f64_x(pg, fb_vec_1, fc_vec_1, fa_vec_1);
      fa_vec_2 = svmad_f64_x(pg, fb_vec_2, fc_vec_2, fa_vec_2);
      fa_vec_3 = svmad_f64_x(pg, fb_vec_3, fc_vec_3, fa_vec_3);
    }
    svst1_f64(pg, &fa[i], fa_vec);
    svst1_f64(pg, &fa[i+svcntd()], fa_vec_1);
    svst1_f64(pg, &fa[i+2*svcntd()], fa_vec_2);
    svst1_f64(pg, &fa[i+3*svcntd()], fa_vec_3);
    // i += svcntd();
    pg = svwhilelt_b64(i, n);
  }
  // while (svptest_any(svptrue_b64(), pg));

#endif

#ifdef INTRINSICS

int64_t i = 0;
  int64_t n = DATA_SIZE;
  svbool_t pg = svwhilelt_b64(i, n);                    // [1]
  // do
  // {
  // #pragma omp simd
  // #pragma unroll
  for( i = 0; i < n; i += svcntd() ) {
    svfloat64_t fa_vec   = svld1(pg, &fa[i]);
    svfloat64_t fb_vec   = svld1(pg, &fb[i]);
    svfloat64_t fc_vec   = svld1(pg, &fc[i]);
        // fa[i] += fc[i] * fb[i];
    for( long t = 0; t < n_trials; t++ ){
      fa_vec = svmad_f64_x(pg, fb_vec, fc_vec, fa_vec);
    }
    svst1_f64(pg, &fa[i], fa_vec);
    // i += svcntd();
    pg = svwhilelt_b64(i, n);
  }
  // while (svptest_any(svptrue_b64(), pg));

#endif



#ifdef TUNED
    
  
// #pragma omp simd
    for (int i = 0; i <= DATA_SIZE - 8; i = i + 8) {
      double scv_1 = fa[i];
      double scv_2 = fa[i+1];
      double scv_3 = fa[i+2];
      double scv_4 = fa[i+3];
      double scv_5 = fa[i+4];
      double scv_6 = fa[i+5];
      double scv_7 = fa[i+6];
      double scv_8 = fa[i+7];
      #pragma omp simd
      for( long t = 0; t < n_trials; t++ ){
        scv_1 += OP(fc[i],fb[i]);
        scv_2 += OP(fc[i+1],fb[i+1]);
        scv_3 += OP(fc[i+2],fb[i+2]);
        scv_4 += OP(fc[i+3],fb[i+3]);
        scv_5 += OP(fc[i+4],fb[i+4]);
        scv_6 += OP(fc[i+5],fb[i+5]);
        scv_7 += OP(fc[i+6],fb[i+6]);
        scv_8 += OP(fc[i+7],fb[i+7]);
      }
      fa[i] = scv_1;
      fa[i+1] = scv_2;
      fa[i+2] = scv_3;
      fa[i+3] = scv_4;
      fa[i+4] = scv_5;
      fa[i+5] = scv_6;
      fa[i+6] = scv_7;
      fa[i+7] = scv_8;
    }

    for (int i = DATA_SIZE - ((DATA_SIZE - (0)) % 8); i <= DATA_SIZE - 1; i = i + 1){
      #pragma omp simd
      for( long t = 0; t < n_trials; t++ ){
        fa[i] += OP(fc[i],fb[i]);
      }
    }



#else
    // int i;
    // Perform multiple trials for timing.
    // #pragma unroll(4096)
    // for( long t = 0; t < n_trials; t++ )
    // {
      // Crunch an FMA.
      // fma(fa, fb, fc, n_trials);

      // #pragma unroll(16)
      // #pragma omp simd simdlen(512)
      // #pragma omp simd
    //   #pragma vector aligned
    //   for (int i = 0; i < DATA_SIZE; i++) {
    //     //fa[i] += fc[i] * fb[i];
    //     fa[i] += OP(fc[i],fb[i]);
    //   }
    // }


    // Perform multiple trials for timing.
  #pragma unroll
  // #pragma omp simd
    for (int i = 0; i < DATA_SIZE; i++) {
      double scv_1;
      scv_1 = fa[i];
      #pragma omp simd
      // #pragma unroll
      for( long t = 0; t < n_trials; t++ ){
        // scv_1 += fc[i] * fb[i];
        scv_1 += OP(fc[i],fb[i]);
      }
      fa[i] = scv_1;
    }



#endif

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

    #pragma omp barrier
    #pragma omp critical
    {
      for (int i = 0; i < DATA_SIZE; i++) {
        norm[tid] += fa[i] * fa[i];
      }

    // Bigger hammer to prevent dead code elimination.
      // FILE *f = fopen( "garbage.out", "a" );
      // if(DATA_SIZE > 32)
      //   fwrite( fa, sizeof(fa[0]), 32, f );
      // else
      //   fwrite( fa, sizeof(fa[0]), DATA_SIZE, f );
      // fclose( f );
    }

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

  if (validate) {
    printf( "%.6f s\n%30s: %.1f GFLOPs\n%30s: %.1f GFLOP/s\n%30s: %.5f \n%30s: %.5f \n\n", 
            t1 - t0, 
            "Flops",
            gflops, 
            "Performance",
            gflops / ( t1 - t0 ),
            "Benchmark Norm",
            norm[5],
            "Theoretical Norm",
            fma_v(n_trials) );
  } else {
    printf( "%.6f s\n%30s: %.1f GFLOPs\n%30s: %.1f GFLOP/s\n%30s: %.5f \n\n", 
            t1 - t0, 
            "Flops",
            gflops, 
            "Performance",
            gflops / ( t1 - t0 ),
            "Benchmark Norm",
            norm[5]);
  }

  // for (int i = 0; i < n_threads; i++) {
  //    printf( "%30s %3d: %.5f\n",  
  //         "Norm Thread ",
  //         i, 
  //         norm[i]);
  // }



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
  input->validate   = FALSE;

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

    if ( !(strcmp("-v", argv[i])) || !(strcmp("--validate", argv[i])) ) {
      input->validate = TRUE;      
    }

  }

}


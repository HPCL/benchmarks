/*-----------------------------------------------------------------------*/
/* Program: STREAM                                                       */
/* Revision: $Id: stream.c,v 5.10 2013/01/17 16:01:06 mccalpin Exp mccalpin $ */
/* Original code developed by John D. McCalpin                           */
/* Programmers: John D. McCalpin                                         */
/*              Joe R. Zagar                                             */
/*                                                                       */
/* This program measures memory transfer rates in MB/s for simple        */
/* computational kernels coded in C.                                     */
/*-----------------------------------------------------------------------*/
/* Copyright 1991-2013: John D. McCalpin                                 */
/*-----------------------------------------------------------------------*/
/* License:                                                              */
/*  1. You are free to use this program and/or to redistribute           */
/*     this program.                                                     */
/*  2. You are free to modify this program for your own use,             */
/*     including commercial use, subject to the publication              */
/*     restrictions in item 3.                                           */
/*  3. You are free to publish results obtained from running this        */
/*     program, or from works that you derive from this program,         */
/*     with the following limitations:                                   */
/*     3a. In order to be referred to as "STREAM benchmark results",     */
/*         published results must be in conformance to the STREAM        */
/*         Run Rules, (briefly reviewed below) published at              */
/*         http://www.cs.virginia.edu/stream/ref.html                    */
/*         and incorporated herein by reference.                         */
/*         As the copyright holder, John McCalpin retains the            */
/*         right to determine conformity with the Run Rules.             */
/*     3b. Results based on modified source code or on runs not in       */
/*         accordance with the STREAM Run Rules must be clearly          */
/*         labelled whenever they are published.  Examples of            */
/*         proper labelling include:                                     */
/*           "tuned STREAM benchmark results"                            */
/*           "based on a variant of the STREAM benchmark code"           */
/*         Other comparable, clear, and reasonable labelling is          */
/*         acceptable.                                                   */
/*     3c. Submission of results to the STREAM benchmark web site        */
/*         is encouraged, but not required.                              */
/*  4. Use of this program or creation of derived works based on this    */
/*     program constitutes acceptance of these licensing restrictions.   */
/*  5. Absolutely no warranty is expressed or implied.                   */
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <sys/time.h>
#include <omp.h>

#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#ifdef USE_LIKWID
#include <likwid-marker.h>
#endif


/*-----------------------------------------------------------------------
 * INSTRUCTIONS:
 *
 *	1) STREAM requires different amounts of memory to run on different
 *           systems, depending on both the system cache size(s) and the
 *           granularity of the system timer.
 *     You should adjust the value of 'STREAM_ARRAY_SIZE' (below)
 *           to meet *both* of the following criteria:
 *       (a) Each array must be at least 4 times the size of the
 *           available cache memory. I don't worry about the difference
 *           between 10^6 and 2^20, so in practice the minimum array size
 *           is about 3.8 times the cache size.
 *           Example 1: One Xeon E3 with 8 MB L3 cache
 *               STREAM_ARRAY_SIZE should be >= 4 million, giving
 *               an array size of 30.5 MB and a total memory requirement
 *               of 91.5 MB.  
 *           Example 2: Two Xeon E5's with 20 MB L3 cache each (using OpenMP)
 *               STREAM_ARRAY_SIZE should be >= 20 million, giving
 *               an array size of 153 MB and a total memory requirement
 *               of 458 MB.  
 *       (b) The size should be large enough so that the 'timing calibration'
 *           output by the program is at least 20 clock-ticks.  
 *           Example: most versions of Windows have a 10 millisecond timer
 *               granularity.  20 "ticks" at 10 ms/tic is 200 milliseconds.
 *               If the chip is capable of 10 GB/s, it moves 2 GB in 200 msec.
 *               This means the each array must be at least 1 GB, or 128M elements.
 *
 *      Version 5.10 increases the default array size from 2 million
 *          elements to 10 million elements in response to the increasing
 *          size of L3 caches.  The new default size is large enough for caches
 *          up to 20 MB. 
 *      Version 5.10 changes the loop index variables from "register int"
 *          to "ssize_t", which allows array indices >2^32 (4 billion)
 *          on properly configured 64-bit systems.  Additional compiler options
 *          (such as "-mcmodel=medium") may be required for large memory runs.
 *
 *      Array size can be set at compile time without modifying the source
 *          code for the (many) compilers that support preprocessor definitions
 *          on the compile line.  E.g.,
 *                gcc -O -DSTREAM_ARRAY_SIZE=100000000 stream.c -o stream.100M
 *          will override the default size of 10M with a new size of 100M elements
 *          per array.
 */
#ifndef STREAM_ARRAY_SIZE
#   define STREAM_ARRAY_SIZE	60000000
#endif

/*  2) STREAM runs each kernel "NTIMES" times and reports the *best* result
 *         for any iteration after the first, therefore the minimum value
 *         for NTIMES is 2.
 *      There are no rules on maximum allowable values for NTIMES, but
 *         values larger than the default are unlikely to noticeably
 *         increase the reported performance.
 *      NTIMES can also be set on the compile line without changing the source
 *         code using, for example, "-DNTIMES=7".
 */
#ifdef NTIMES
#if NTIMES<=1
#   define NTIMES       10
#endif
#endif

#ifndef NTIMES
#   define NTIMES       10
#endif

#ifndef NTIMES_KK
#   define NTIMES_KK    10
#endif


/*  Users are allowed to modify the "OFFSET" variable, which *may* change the
 *         relative alignment of the arrays (though compilers may change the 
 *         effective offset by making the arrays non-contiguous on some systems). 
 *      Use of non-zero values for OFFSET can be especially helpful if the
 *         STREAM_ARRAY_SIZE is set to a value close to a large power of 2.
 *      OFFSET can also be set on the compile line without changing the source
 *         code using, for example, "-DOFFSET=56".
 */
#ifndef OFFSET
#   define OFFSET	0
#endif

/*
 *	3) Compile the code with optimization.  Many compilers generate
 *       unreasonably bad code before the optimizer tightens things up.  
 *     If the results are unreasonably good, on the other hand, the
 *       optimizer might be too smart for me!
 *
 *     For a simple single-core version, try compiling with:
 *            cc -O stream.c -o stream
 *     This is known to work on many, many systems....
 *
 *     To use multiple cores, you need to tell the compiler to obey the OpenMP
 *       directives in the code.  This varies by compiler, but a common example is
 *            gcc -O -fopenmp stream.c -o stream_omp
 *       The environment variable OMP_NUM_THREADS allows runtime control of the 
 *         number of threads/cores used when the resulting "stream_omp" program
 *         is executed.
 *
 *     To run with single-precision variables and arithmetic, simply add
 *         -DSTREAM_TYPE=float
 *     to the compile line.
 *     Note that this changes the minimum array sizes required --- see (1) above.
 *
 *     The preprocessor directive "TUNED" does not do much -- it simply causes the 
 *       code to call separate functions to execute each kernel.  Trivial versions
 *       of these functions are provided, but they are *not* tuned -- they just 
 *       provide predefined interfaces to be replaced with tuned code.
 *
 *
 *	4) Optional: Mail the results to mccalpin@cs.virginia.edu
 *	   Be sure to include info that will help me understand:
 *		a) the computer hardware configuration (e.g., processor model, memory type)
 *		b) the compiler name/version and compilation flags
 *      c) any run-time information (such as OMP_NUM_THREADS)
 *		d) all of the output from the test case.
 *
 * Thanks!
 *
 *-----------------------------------------------------------------------*/

# define HLINE "-------------------------------------------------------------\n"

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif

#ifndef STREAM_TYPE
#define STREAM_TYPE double
#endif
#define ALIGN 64
typedef STREAM_TYPE __attribute__((aligned(ALIGN))) array_type;
// #define array_type double


#define SCALAR 3.0

// static STREAM_TYPE	a[STREAM_ARRAY_SIZE+OFFSET],
// 			b[STREAM_ARRAY_SIZE+OFFSET],
// 			c[STREAM_ARRAY_SIZE+OFFSET];


static double   avgtime[4] = { 0 },
                maxtime[4] = { 0 },
                mintime[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };

  size_t              array_elements, array_bytes, array_alignment;

static char     *label[4] = { "Copy:      ",
                              "Scale:     ",
                              "Add:       ",
                              "Triad:     " };


static double	bytes[4] = {
    2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE
    };



extern void computeSTREAMerrors( array_type * restrict a,
                                 array_type * restrict b,
                                 array_type * restrict c,
                                 STREAM_TYPE *aAvgErr,
                                 STREAM_TYPE *bAvgErr,
                                 STREAM_TYPE *cAvgErr );

extern void checkSTREAMresults( STREAM_TYPE * restrict a,
                                STREAM_TYPE * restrict b,
                                STREAM_TYPE * restrict c,
                                STREAM_TYPE *AvgErrByThr,
                                int numranks );

extern double mysecond();

#ifdef TUNED
extern void tuned_STREAM_Copy(array_type* restrict a,
                              array_type* restrict b,
                              array_type* restrict c,
                              int array_size);
extern void tuned_STREAM_Scale(array_type* restrict a,
                              array_type* restrict b,
                              array_type* restrict c,
                              STREAM_TYPE scalar,
                              int array_size);
extern void tuned_STREAM_Add(array_type* restrict a,
                              array_type* restrict b,
                              array_type* restrict c,
                              int array_size);
extern void tuned_STREAM_Triad(array_type* restrict a,
                              array_type* restrict b,
                              array_type* restrict c,
                              STREAM_TYPE scalar,
                              int array_size);
#endif

#ifdef _OPENMP
extern int omp_get_num_threads();
#endif

int
main()
    {

    /* --- SETUP --- determine precision and check timing --- */

#ifdef USE_CALI
cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);
#endif
  int                 t;
  int                 num_thr;
  STREAM_TYPE         *AvgErrByThr;

  #pragma omp parallel shared(array_elements, array_bytes, array_alignment, num_thr)
  {

  // Some compilers require an extra keyword to recognize the "restrict" qualifier.
  // array_type* restrict a;
  // array_type* restrict b;
  // array_type* restrict c;
  STREAM_TYPE* restrict a;
  STREAM_TYPE* restrict b;
  STREAM_TYPE* restrict c;
  int                 quantum, checktick();
  int                 BytesPerWord;
  int                 thr_id;
  int                 k, i, kk;
  ssize_t             j;
  STREAM_TYPE         scalar;
  double              times[ 4 ][ NTIMES ]; // private list of times

  double              *TimesByRank;
  double              t0, t1, tmin;

  STREAM_TYPE         AvgError[3] = { 0.0, 0.0, 0.0 };

  thr_id  = omp_get_thread_num();

  if ( thr_id == 0 )
  {
  num_thr = omp_get_num_threads();
  /* --- DAVE FEATURE --- distribute requested storage across MPI ranks --- */
  array_elements  = STREAM_ARRAY_SIZE / num_thr; // Do not worry about rounding vs truncation.
  array_alignment = ALIGN;                           // Can be modified -- provides partial support
                                                  // for adjusting relative alignment.

  // Dynamically allocate the three arrays using "posix_memalign()".
  // NOTE that the OFFSET parameter is not used in this version of the code.
  array_bytes = array_elements * sizeof( STREAM_TYPE );
  }
  #pragma omp barrier

  k = -1;
  a = (double*)aligned_alloc(array_alignment,array_bytes);
  // k = posix_memalign( (void **) &a, array_alignment, array_bytes );
  if ( a == 0 ) {
    printf( "Thread %d: Allocation of array a failed, return code is %d\n",
            thr_id,
            k );
    exit( 1 );
  }

  b = (double*)aligned_alloc(array_alignment,array_bytes);
  // k = posix_memalign( (void **) &b, array_alignment, array_bytes );
  if ( b == 0 )
  {
    printf( "Thread %d: Allocation of array b failed, return code is %d\n",
            thr_id,
            k );
    exit( 1 );
  }

  c = (double*)aligned_alloc(array_alignment,array_bytes);
  // k = posix_memalign( (void **) &c, array_alignment, array_bytes );
  if ( c == 0 )
  {
    printf( "Thread %d: Allocation of array c failed, return code is %d\n",
            thr_id,
            k );
    exit( 1 );
  }

  for (k=0; k<NTIMES; k++) {
      for (j=0; j<4; j++) {
        times[j][k] = -1.0;
      }
  }

  for( j = 0; j < array_elements; j++ )
  {
    a[j] = 1.0;
    b[j] = 2.0;
    c[j] = 0.0;
  }

  if ( thr_id == 0 )
  {
    printf(HLINE);
    printf("STREAM version $Revision: 5.10 $\n");
    printf(HLINE);
    BytesPerWord = sizeof(STREAM_TYPE);
    printf("This system uses %d bytes per array element.\n",
	BytesPerWord);

    printf(HLINE);
#ifdef N
    printf("*****  WARNING: ******\n");
    printf("      It appears that you set the preprocessor variable N when compiling this code.\n");
    printf("      This version of the code uses the preprocesor variable STREAM_ARRAY_SIZE to control the array size\n");
    printf("      Reverting to default value of STREAM_ARRAY_SIZE=%llu\n",(unsigned long long) STREAM_ARRAY_SIZE);
    printf("*****  WARNING: ******\n");
#endif

    printf("Total Elements (all threads) = %llu (elements), Offset = %d (elements)\n" , (unsigned long long) array_elements, OFFSET);
    printf("Memory per array per thread = %.1f MiB (= %.1f GiB).\n", 
	BytesPerWord * ( (double) array_elements / 1024.0/1024.0),
	BytesPerWord * ( (double) array_elements / 1024.0/1024.0/1024.0));
    printf("Total memory required = %.1f MiB (= %.1f GiB).\n",
	(3.0 * BytesPerWord) * ( (double) STREAM_ARRAY_SIZE / 1024.0/1024.),
	(3.0 * BytesPerWord) * ( (double) STREAM_ARRAY_SIZE / 1024.0/1024./1024.));
    printf("Each kernel will be executed %d times.\n", NTIMES);
    printf(" The *best* time for each kernel (excluding the first iteration)\n"); 
    printf(" will be used to compute the reported bandwidth.\n");

    printf(HLINE);
    t = omp_get_num_threads();
    printf ("Number of Threads requested = %i\n",t);
    t = 0;
  } // if thr_id == 0


#pragma omp barrier 
#pragma omp atomic 
        t++;
#pragma omp barrier 

  if ( thr_id == 0 )
  {    
    printf ("Number of Threads counted = %i\n",t);
  }
  
#pragma omp barrier 
  

#ifdef USE_CALI
  #ifdef USE_CALI_UNCORE
  if (thr_id == 0)
  #endif
cali_set_int(thread_attr, omp_get_thread_num());
#endif
    
#ifdef USE_LIKWID
LIKWID_MARKER_INIT;
#endif


  // Defining error variables TODO - do we need this?
  if ( thr_id == 0 )
  {
    // There are 3 average error values for each rank (using STREAM_TYPE).
    AvgErrByThr = (STREAM_TYPE *) malloc( 3 * sizeof( STREAM_TYPE ) * num_thr );

    if ( AvgErrByThr == NULL )
    {
      printf( "Ooops -- allocation of arrays to collect errors on MPI rank 0 failed\n" );
    }

    memset( AvgErrByThr, 0, 3 * sizeof( STREAM_TYPE ) * num_thr );

    // There are 4*NTIMES timing values for each rank (always doubles)
    TimesByRank = (double *) malloc( 4 * NTIMES * sizeof(double) * num_thr );

    if ( TimesByRank == NULL )
    {
      printf( "Ooops -- allocation of arrays to collect timing data on MPI rank 0 failed\n" );
    }

    memset( TimesByRank, 0, 4 * NTIMES * sizeof(double) * num_thr );
  } // if thr_id == 0

  // Simple check for granularity of the timer being used
  if ( thr_id == 0 )
  {
    printf( HLINE );

    if ( ( quantum = checktick() ) >= 1 )
    {
      printf( "Your timer granularity/precision appears to be "
              "%d microseconds.\n",
              quantum );
    }

    else
    {
      printf( "Your timer granularity appears to be "
              "less than one microsecond.\n" );

      quantum = 1;
    }

    t = mysecond();
    for (j = 0; j < array_elements; j++)
		a[j] = 2.0E0 * a[j];
    t = 1.0E6 * (mysecond() - t);

    // reset
    for( j = 0; j < array_elements; j++ )
        a[j] = 1.0;

    printf("Each test below will take on the order"
	" of %d microseconds.\n", (int) t  );
    printf("   (= %d clock ticks)\n", (int) (t/quantum) );
    printf("Increase the size of the arrays if this shows that\n");
    printf("you are not getting at least 20 clock ticks per test.\n");

    printf(HLINE);

    printf("WARNING -- The above is only a rough guideline.\n");
    printf("For best results, please be sure you know the\n");
    printf("precision of your system timer.\n");
    printf(HLINE);
  } // if thr_id == 0
    

  //---------------------------------------------------------------------
  // Run through the tests once without any timing or collection of
  // hardware counters in order to load data into the cache when doing
  // cache resident runs.
  //---------------------------------------------------------------------

  //---------------------------------------------------------------------
  // Kernel 1: Copy
  //---------------------------------------------------------------------

  #ifdef COPY
  #ifdef TUNED
  tuned_STREAM_Copy(a,b,c,array_elements);
  #else
  #pragma omp simd
  for( j = 0; j < array_elements; j++ )
  {
    c[j] = a[j];
  }
  #endif
  #endif

  //---------------------------------------------------------------------
  // Kernel 2: Scale
  //---------------------------------------------------------------------

  #ifdef SCALE
  #ifdef TUNED
  tuned_STREAM_Scale( a,b,c,scalar,array_elements );
  #else
  #pragma omp simd
  for( j = 0; j < array_elements; j++ )
  {
    b[j] = scalar * c[j];
  }
  #endif
  #endif

  //---------------------------------------------------------------------
  // Kernel 3: Add
  //---------------------------------------------------------------------

  #ifdef ADD
  #ifdef TUNED
  tuned_STREAM_Add(a,b,c,array_elements);
  #else
  #pragma omp simd
  for( j = 0; j < array_elements; j++ )
  {
    c[j] = a[j] + b[j];
  }
  #endif
  #endif

  //---------------------------------------------------------------------
  // Kernel 4: Triad
  //---------------------------------------------------------------------

  #ifdef TRIAD
  // #ifdef TUNED
  // tuned_STREAM_Triad( a,b,c,scalar,array_elements );
  // #else
  #pragma omp simd
  for( j = 0; j < array_elements; j++ )
  {
    a[j] = b[j] + scalar * c[j];
  }
  // #endif
  #endif


    /*	--- MAIN LOOP --- repeat test cases NTIMES times --- */


  //---------------------------------------------------------------------
  // Configure beginning of LikWid measurement region.
  //---------------------------------------------------------------------

  #ifdef STREAM_USE_LIKWID

  #if defined COPY
  LIKWID_MARKER_INIT;
  LIKWID_MARKER_START("stream_copy");

  #elif defined SCALE
  LIKWID_MARKER_INIT;
  LIKWID_MARKER_START("stream_scale");

  #elif defined ADD
  LIKWID_MARKER_INIT;
  LIKWID_MARKER_START("stream_add");

  #elif defined TRIAD
  LIKWID_MARKER_INIT;
  LIKWID_MARKER_START("stream_triad");

  #else
  LIKWID_MARKER_INIT;
  LIKWID_MARKER_START("stream_all");

  #endif

  #endif

  //---------------------------------------------------------------------
  // Configure beginning of Caliper measurement region.
  //---------------------------------------------------------------------

  #ifdef USE_CALI
#ifdef USE_CALI_UNCORE
if (thr_id == 0)
{
#endif
  #if defined COPY
  CALI_MARK_BEGIN("stream_copy");

  #elif defined SCALE
  CALI_MARK_BEGIN("stream_scale");

  #elif defined ADD
  CALI_MARK_BEGIN("stream_add");

  #elif defined TRIAD
  CALI_MARK_BEGIN("stream_triad");

  #else
  CALI_MARK_BEGIN("stream_all");

  #endif
#ifdef USE_CALI_UNCORE
}
#endif

  #endif

    scalar = SCALAR;
    for (k=0; k<NTIMES; k++)
	{

    #ifdef COPY
    // COPY
	times[0][k] = mysecond();
    for( kk = 0; kk < NTIMES_KK; kk++ )
    {
    #ifdef TUNED
    tuned_STREAM_Copy(a,b,c,array_elements);
    #else
	for (j=0; j<array_elements; j++)
	    c[j] = a[j];
    #endif
    }
	times[0][k] = mysecond() - times[0][k];
    #endif

    #ifdef SCALE
    // SCALE
	times[1][k] = mysecond();
    for( kk = 0; kk < NTIMES_KK; kk++ )
    {
    #ifdef TUNED
    tuned_STREAM_Scale(a,b,c,scalar,array_elements);
    #else
	for (j=0; j<array_elements; j++)
	    b[j] = scalar*c[j];
    #endif
    }
	times[1][k] = mysecond() - times[1][k];
    #endif

    #ifdef ADD
    // ADD
	times[2][k] = mysecond();
    for( kk = 0; kk < NTIMES_KK; kk++ )
    {
    #ifdef TUNED
        tuned_STREAM_Add(a,b,c);
    #else
	for (j=0; j<array_elements; j++)
	    c[j] = a[j]+b[j];
    #endif
    }
	times[2][k] = mysecond() - times[2][k];
    #endif

    #ifdef TRIAD
    // TRIAD
	times[3][k] = mysecond();
    for( kk = 0; kk < NTIMES_KK; kk++ )
    {
    #ifdef TUNED
    tuned_STREAM_Triad(a,b,c,scalar,array_elements);
    #else
	for (j=0; j<array_elements; j++)
	    a[j] = b[j]+scalar*c[j];
    #endif
    }
	times[3][k] = mysecond() - times[3][k];
    #endif

	} //  NTIMES loop


  //---------------------------------------------------------------------
  // Configure end of Caliper measurement region.
  //---------------------------------------------------------------------

#pragma omp barrier 
  #ifdef USE_CALI
#ifdef USE_CALI_UNCORE
if (thr_id == 0)
{
#endif
  #if defined COPY
  CALI_MARK_END("stream_copy");

  #elif defined SCALE
  CALI_MARK_END("stream_scale");

  #elif defined ADD
  CALI_MARK_END("stream_add");

  #elif defined TRIAD
  CALI_MARK_END("stream_triad");

  #else
  CALI_MARK_END("stream_all");

  #endif
#ifdef USE_CALI_UNCORE
}
#endif
  #endif

  #ifdef STREAM_USE_LIKWID

  #if defined COPY
  LIKWID_MARKER_STOP("stream_copy");
  LIKWID_MARKER_CLOSE;

  #elif defined SCALE
  LIKWID_MARKER_STOP("stream_scale");
  LIKWID_MARKER_CLOSE;

  #elif defined ADD
  LIKWID_MARKER_STOP("stream_add");
  LIKWID_MARKER_CLOSE;

  #elif defined TRIAD
  LIKWID_MARKER_STOP("stream_triad");
  LIKWID_MARKER_CLOSE;

  #else
  LIKWID_MARKER_STOP("stream_all");
  LIKWID_MARKER_CLOSE;

  #endif

  #endif
  

    /*	--- SUMMARY --- */

#pragma omp critical
  {
    for (k=1; k<NTIMES; k++) /* note -- skip first iteration which is not measured */
	{

	for (j=0; j<4; j++) {

	    avgtime[j] = avgtime[j] + times[j][k];
	    mintime[j] = MIN(mintime[j], times[j][k]);
	    maxtime[j] = MAX(maxtime[j], times[j][k]);
	}

	}
  }
    
    //TODO reduce times; averages over runs and threads

    //TODO add check STREAM Error
    // We can probably do this the same as the errors

  }  // end omp parallel


    printf("Function    Best Rate MB/s  Avg time     Min time     Max time\n");
    for (int j=0; j<4; j++) {
		avgtime[j] = avgtime[j]/((double)(NTIMES-1)*(double)num_thr);

		printf("%s%12.1f  %11.6f  %11.6f  %11.6f\n", label[j],
	       1.0E-06 * bytes[j] * NTIMES_KK/mintime[j],
	       avgtime[j],
	       mintime[j],
	       maxtime[j]);
    }
    printf(HLINE);

    /* --- Check Results --- */
    // checkSTREAMresults();
    printf(HLINE);

    return 0;
}



/***************************************************************************************\


                    TUNED KERNELS HERE


\***************************************************************************************/


#ifdef TUNED
/* stubs for "tuned" versions of the kernels */
void tuned_STREAM_Copy(array_type* restrict a,
                       array_type* restrict b,
                       array_type* restrict c,
                       int array_size)
{
	ssize_t j, _b, kk;

  // for( kk = 0; kk < NTIMES_KK; kk++ )
    #pragma omp simd
    for (j=0; j<array_size; j++)
      c[j] = a[j];

}

void tuned_STREAM_Scale(array_type* restrict a,
                        array_type* restrict b,
                        array_type* restrict c,
                        STREAM_TYPE scalar,
                        int array_size)
{
	ssize_t j, _b, kk;

  // for( kk = 0; kk < NTIMES_KK; kk++ )
    #pragma omp simd
    for (j=0; j<array_size; j++)
      b[j] = scalar*c[j];
}

void tuned_STREAM_Add(array_type* restrict a,
                      array_type* restrict b,
                      array_type* restrict c,
                      int array_size)
{
	ssize_t j, _b, kk;
  // for( kk = 0; kk < NTIMES_KK; kk++ )
    #pragma omp simd
    for (j=0; j<array_size; j++)
      c[j] = a[j]+b[j];

}

void tuned_STREAM_Triad(array_type* restrict a,
                        array_type* restrict b,
                        array_type* restrict c,
                        STREAM_TYPE scalar,
                        int array_size)
{
	ssize_t j, _b, kk;
  // for( kk = 0; kk < NTIMES_KK; kk++ )
    #pragma omp simd
    for (j=0; j<array_size; j++)
      a[j] = b[j]+scalar*c[j];

}
/* end of stubs for the "tuned" versions of the kernels */
#endif


/***************************************************************************************\


                    ERROR CHECKING HERE


\***************************************************************************************/


#ifndef abs
#define abs(a) ((a) >= 0 ? (a) : -(a))
#endif

void computeSTREAMerrors( array_type* restrict a,
                          array_type* restrict b,
                          array_type* restrict c,
                          STREAM_TYPE *aAvgErr,
                          STREAM_TYPE *bAvgErr,
                          STREAM_TYPE *cAvgErr )
{
  STREAM_TYPE aj, bj, cj, scalar;
  STREAM_TYPE aSumErr, bSumErr, cSumErr;
  ssize_t     j;
  int         k;

  /* reproduce initialization */
  aj = 1.0;
  bj = 2.0;
  cj = 0.0;

  /* a[] is modified during timing check */
  aj = 2.0e0 * aj;

  /* now execute timing loop */
  scalar = SCALAR;

  for( k = 0; k < NTIMES; k++ )
  {
    #ifdef COPY
    cj = aj;
    #endif

    #ifdef SCALE
    bj = scalar * cj;
    #endif

    #ifdef ADD
    cj = aj + bj;
    #endif

    #ifdef TRIAD
    aj = bj + scalar * cj;
    #endif
  }

  /* accumulate deltas between observed and expected results */
  aSumErr = 0.0;
  bSumErr = 0.0;
  cSumErr = 0.0;

  for( j = 0; j < array_elements; j++ )
  {
    aSumErr += abs( a[j] - aj );
    bSumErr += abs( b[j] - bj );
    cSumErr += abs( c[j] - cj );
  }

  *aAvgErr = aSumErr / (STREAM_TYPE) array_elements;
  *bAvgErr = bSumErr / (STREAM_TYPE) array_elements;
  *cAvgErr = cSumErr / (STREAM_TYPE) array_elements;
}



void checkSTREAMresults( array_type* restrict a,
                         array_type* restrict b,
                         array_type* restrict c,
                         STREAM_TYPE *AvgErrByThr,
                         int numranks )
{
  STREAM_TYPE aj, bj, cj, scalar;
  STREAM_TYPE aSumErr, bSumErr, cSumErr;
  STREAM_TYPE aAvgErr, bAvgErr, cAvgErr;
  ssize_t     j;
  int         k, ierr, err;
  double      epsilon;

  /* reproduce initialization */
  aj = 1.0;
  bj = 2.0;
  cj = 0.0;

  /* a[] is modified during timing check */
  /* this has been reset in the current version but that may need to return */
  // aj = 2.0e0 * aj;

  /* now execute timing loop */
  scalar = SCALAR;

  for( k = 0; k < NTIMES; k++ )
  {
    cj = aj;
    bj = scalar * cj;
    cj = aj + bj;
    aj = bj + scalar * cj;
  }

  // Compute the average of the average errors contributed by each MPI rank
  aSumErr = 0.0;
  bSumErr = 0.0;
  cSumErr = 0.0;

  for( k = 0; k < numranks; k++ )
  {
    aSumErr += AvgErrByThr[ 3 * k + 0 ];
    bSumErr += AvgErrByThr[ 3 * k + 1 ];
    cSumErr += AvgErrByThr[ 3 * k + 2 ];
  }

  aAvgErr = aSumErr / (STREAM_TYPE) numranks;
  bAvgErr = bSumErr / (STREAM_TYPE) numranks;
  cAvgErr = cSumErr / (STREAM_TYPE) numranks;

  if ( sizeof( STREAM_TYPE ) == 4 )
  {
    epsilon = 1.0e-6;
  }

  else if ( sizeof( STREAM_TYPE ) == 8 )
  {
    epsilon = 1.0e-13;
  }

  else
  {
    printf( "WEIRD: sizeof(STREAM_TYPE) = %lu\n",
            sizeof( STREAM_TYPE ) );

    epsilon = 1.0e-6;
  }

  err = 0;

  if ( abs( aAvgErr / aj ) > epsilon )
  {
    err++;

    printf( "Failed Validation on array a[], AvgRelAbsErr > epsilon (%e)\n",
            epsilon );
    printf( "     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",
            aj,
            aAvgErr,
            abs( aAvgErr ) / aj );

    ierr = 0;

    for( j = 0; j < array_elements; j++ )
    {
      if ( abs( a[j] / aj - 1.0 ) > epsilon )
      {
        ierr++;

        #ifdef VERBOSE
        if ( ierr < 10 )
        {
          printf( "         array a: index: %ld, expected: %e, observed: %e, relative error: %e\n",
                  j,
                  aj,
                  a[j],
                  abs( ( aj - a[j] ) / aAvgErr ) );
        }
        #endif
      }
    }
    printf( "     For array a[], %d errors were found.\n",
            ierr );
  }

  if ( abs( bAvgErr / bj ) > epsilon )
  {
    err++;

    printf( "Failed Validation on array b[], AvgRelAbsErr > epsilon (%e)\n",
            epsilon );
    printf( "     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",
            bj,
            bAvgErr,
            abs( bAvgErr ) / bj );
    printf( "     AvgRelAbsErr > Epsilon (%e)\n",
            epsilon );

    ierr = 0;

    for( j = 0; j < array_elements; j++ )
    {
      if ( abs( b[j] / bj - 1.0 ) > epsilon )
      {
        ierr++;

        #ifdef VERBOSE
        if ( ierr < 10 )
        {
          printf( "         array b: index: %ld, expected: %e, observed: %e, relative error: %e\n",
                  j,
                  bj,
                  b[j],
                  abs( ( bj - b[j] ) / bAvgErr ) );
        }
        #endif
      }
    }
    printf( "     For array b[], %d errors were found.\n",
            ierr );
  }

  if ( abs( cAvgErr / cj ) > epsilon )
  {
    err++;

    printf( "Failed Validation on array c[], AvgRelAbsErr > epsilon (%e)\n",
            epsilon );
    printf( "     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",
            cj,
            cAvgErr,
            abs( cAvgErr ) / cj );
    printf( "     AvgRelAbsErr > Epsilon (%e)\n",
            epsilon );

    ierr = 0;

    for( j = 0; j < array_elements; j++ )
    {
      if ( abs( c[j] / cj - 1.0 ) > epsilon )
      {
        ierr++;

        #ifdef VERBOSE
        if ( ierr < 10 )
        {
          printf( "         array c: index: %ld, expected: %e, observed: %e, relative error: %e\n",
                  j,
                  cj,
                  c[j],
                  abs( ( cj - c[j] ) / cAvgErr ) );
        }
        #endif
      }
    }
    printf( "     For array c[], %d errors were found.\n",
            ierr );
  }

  if ( err == 0 )
  {
    printf( "Solution Validates: avg error less than %e on all three arrays\n",
            epsilon );
  }

  #ifdef VERBOSE
  printf( "Results Validation Verbose Results: \n" );
  printf( "    Expected a(1), b(1), c(1): %f %f %f \n",
          aj,
          bj,
          cj );
  printf( "    Observed a(1), b(1), c(1): %f %f %f \n",
          a[1],
          b[1],
          c[1] );
  printf( "    Rel Errors on a, b, c:     %e %e %e \n",
          abs( aAvgErr / aj ),
          abs( bAvgErr / bj ),
          abs( cAvgErr / cj ) );
  #endif
}



/***************************************************************************************\


                    TIMING TESTING FUNCITONS


\***************************************************************************************/





# define    M   20

int
checktick()
    {
    int     i, minDelta, Delta;
    double  t1, t2, timesfound[M];

/*  Collect a sequence of M unique time values from the system. */

    for (i = 0; i < M; i++) {
    t1 = mysecond();
    while( ((t2=mysecond()) - t1) < 1.0E-6 )
        ;
    timesfound[i] = t1 = t2;
    }

/*
 * Determine the minimum difference between these M values.
 * This result will be our estimate (in microseconds) for the
 * clock granularity.
 */

    minDelta = 1000000;
    for (i = 1; i < M; i++) {
    Delta = (int)( 1.0E6 * (timesfound[i]-timesfound[i-1]));
    minDelta = MIN(minDelta, MAX(Delta,0));
    }

   return(minDelta);
} // check tick



/* A gettimeofday routine to give access to the wall
   clock timer on most UNIX-like systems.  */

#include <sys/time.h>

double mysecond()
{
        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


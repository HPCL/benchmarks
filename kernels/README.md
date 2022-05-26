# Kernels
A set of micro-benchmarks and kernels to aid in developing and validating performance analysis techniques. Each kernel is instrumented using the Caliper performance measurement tool, and we provide scripts to help collect relavant hardware counter data on Intel Cascadelake and Fujitsu A64FX CPUs.

## STREAM
- A copy of the stream benchmark with instrumentation for hardware counters
- see https://github.com/jeffhammond/STREAM
- or https://www.cs.virginia.edu/stream/


## mm_ex
- our MPI based general matrix matrix multiplication

## mm_blocks
- alternative matrix multiplcation implementation
- based on OpenMP with variations


## stencil_ex
- our stencil-based benchmark
- useful for testing performance analysis techniques with code that is slightly more interesting than mm


## cache_metrics
- collection of benchmarks to measure cache metrics, or validate counters
- cache conflict - benchmark to measure cache conflicts 
- cache misses - attempt to benchmark predictable cache misses (incomplete)
- byte validation - attempt to benchmark predictable volumes of byte movement (incomplete)

## n_body
- implentation of N Body kernel computing gavitational forces on celestial bodies
- parallel implementation through OpenMP


## shingles
- two kernels to measure the performance of the floating point and memory operations
  - fp crunch - performs repeated floating point operations to excercise the arithmetic units
  - STREAM - modified STREAM to measure apparent bandwidth between CPUs and cache levels


## boundness_measure
- depricated
- originally intended to benchmark the relative boundness of kernels based on arithmetic intensity
- abandoned in favor of the shingles approach


## cache_misses
- depricated
- Originally intended to validate cache miss hardware counters
- abandoned in favor of the shingles approach and cache metrics

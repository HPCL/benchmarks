# Boundness Micro-Benchmark

This micro benchmark is designed to provide several small kernels of C that are
known to be bound by computation or memory use. It can then be used to validate
hardware performance counters on an architecture for future use analyzing more
useful applications.

## Design
The micro-benchmark runs through an array add repeatly adds elements of the 
array to themselves. The array can be sized to fit the L1 or L2 cache and the
number of FLOPS (times that the element is added to itself) can be adjusted as
well. These parameters allow the user to easily scale the amount of computation
and memory operations to study how performance counters react on different CPU
architectures.


## Usage

./measure_bounds -[c] -t [t] -f [f]

c is for the data size
  * -1 size is several times L1 but blocked to fit in L1
  * -2 size is equal to L2
  * -3 size is equal to L3
  * -m size is 3x L3 and broken because it is currently block to L3

t is for the number of threads
  * we run the number of memory controllers
  * 12 for Skylake and 8 for Rome

f is for the flops
  * divide by 8 to get arithmetic intensity

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

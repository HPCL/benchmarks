# Cache Metrics

These micro-benchmarks are designed to produce predictable events in caches. These predictable events allow the users to test hardware counters and explore the performance of caches.  



## Cache Conflict
The benchmark iterates through an array that is several times larger than the L1 cache. The step size of the iterations is adjustable. By adjusting the offset between accesses, we can create or eliminate the conflicts in cache lines or produce an abundance of them which allows us to examine how hardware counter respond to cache conflicts. This kernel is single threaded as it focuses on L1 cache impacts, but vaiations for other caches would benefit from adding threading.

### Usage
The kernel can be run as shown below. The offset is the iteration step size to be varied. 

./cache_conflict -o [integer offest]




## Miss Counts
The miss count micro benchmark is designed to validate miss counts at each level of cache. The kernel performs a matrix addition on two matrices. The matrices can be resized to fit into the different caches which allows the user to explore different expected miss counts. This benchmark is incomplete.

### Usage
./miss_count -[c] -t [t]

c is for the data size
  * -1 size is several times L1 but blocked to fit in L1
  * -2 size is equal to L2
  * -3 size is equal to L3

t is for the number of threads




## Byte Validation
The data movement validation benchmark is designed as an extended version of STREAM Triad. We include one part which is intended to use SIMD operations based on compiler support and one part which performs a reduction. The number of operations performed in the SIMD loop and in the reduction loop can each be varied independantly. Memory movement from these types of load and store operations are not clearly defined in hardware counter documentation. Once this benchmark is complete it should help validate the counters used to measure data movement.

### Usage
./byte_validation -s [s] -r [r]

s - number of SIMD operations

r - number of reduciton operations




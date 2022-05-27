# MM Blocks
A wide range of matrix multiplication variations to explore and validate performance measurement methods. The kernel performs a dense matrix multiplication between two matrices and store the result in a third martix. By default all three matrices are 8192 by 8192 double precision floating point elements.



## Matrix multiplication vaiations
We include a range of implementations of the matrix multiplcation which allow the user to examine how a new method of performance analysis works when applied to a known situation.


### Default
By default, the matrices are all arranged so that the elements are aligned in memory so that the elements in each row are adjacent in memory. As a result, the second input matrix has a poor memory access pattern, only using one matrix element of those loaded in a cache line.

### Transpose
The first optimization is to transpose the second matrix to improve the memory access pattern. 

### UNROLLJAM
The second optimization is to unroll the inner loop which can improve IPC and SIMD performance.

### BLOCKED
The inner two loops are blocked to improve the cache performance.

### BLOCKED_INTERCHANGE
All three loops are blocked for the cache performance.

### BLOCK_UNROLL
The inner two loops are blocked to improve the cache performance and the innermost is unrolled

### GAP
Some of the versions can also use a gapping in the matrix allocation. This could be useful for aligning the memory with cache lines or for avoiding conflicts and false sharing.

### INTERCHANGE
This version is similar to transpose, but has the loop ordering swapped. This change negatively impacts performance and memory access which makes it a useful example in some cases.


## Useage
make ORDER=8192 BLOCK_ROWS=256 BLOCK_COLS=256 D=1
./test_linear_d.out

See the makefile for the full set of options along with the executable name that will be produced.

To change the number of threads or parallelism settings, use OMP environment variables.


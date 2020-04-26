# LAPACK eigensolvers benchmark 

This benchmark is focused or fairly small (up to order-5000) complex Hermitian matrices.

To build and run with default values:
```
mkdir build
cd build
cmake ..
make 
./diag.exe
```

You can modify the size of the matrix by editing ```src/diag.f90``` and changing the value of *N*.

To enable specific LAPACK/BLAS versions, use the CMake ```BLA_VENDOR``` and ```CMAKE_PREFIX_PATH``` variable, for example, for OpenBLAS:

```
cmake -DBLA_VENDOR=OpenBLAS -DCMAKE_PREFIX_PATH=/path/to/openblas/dir
```

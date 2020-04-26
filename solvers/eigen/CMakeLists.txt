cmake_minimum_required(VERSION 3.16)
project(diag.exe Fortran)

set(CMAKE_CXX_STANDARD 14)

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

add_executable(diag.exe src/diag.f90)
target_link_libraries(diag.exe ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})

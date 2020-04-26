cmake_minimum_required(VERSION 3.14)
project(diag.exe Fortran)

set (CMAKE_BUILD_TYPE RELWITHDEBINFO)
if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
    set(dialect "-fdefault-integer-8 -ffree-form -std=f2008 -fimplicit-none")
    set(bounds "-fbounds-check")
endif()
if(CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
    set(dialect "-g -O2 -i8 -stand f08 -free -implicitnone")
    set(bounds "-check bounds")
endif()
if(CMAKE_Fortran_COMPILER_ID STREQUAL "PGI")
    set(dialect "-g -O2 -Mfreeform -Mdclchk -Mstandard -Mallocatable=03")
    set(bounds "-C")
endif()

# https://cmake.org/cmake/help/latest/module/FindBLAS.html
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

add_executable(diag.exe src/diag.f90)
target_link_libraries(diag.exe ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})

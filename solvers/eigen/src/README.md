MAGMADIR=${HOME}/compil/magma-2.5.3/
ifort  -O3 -g -I${MAGMADIR}include -L${MAGMADIR}lib -o magma magma.f90 -mkl -lmagma

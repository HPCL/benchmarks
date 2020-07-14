# matrix multiplication with openmp intended as a Caliper test

#CC=g++
CC=icpc
INC=
LIB=-fopenmp

CFLAGS = -g
CFLAGS += -qopt-streaming-stores=never

#USE_CALI=1
#USE_LIKWID=1


#X_SIZE=7000

X_SIZE=3500
Y_SIZE=3000
# X_SIZE=15
# Y_SIZE=10

# STENCIL_TYPE=0 # 1 deep halo
#STENCIL_TYPE=1 # 1 deep cross
#STENCIL_TYPE=2 # 2 deep cross
STENCIL_TYPE=3 # 4 deep cross
STENCIL_TYPE=4 # 4 deep halo
#

VARS=-DX_SIZE=${X_SIZE} 
VARS+=-DY_SIZE=${Y_SIZE} 
VARS+=-DSTENCIL_TYPE=${STENCIL_TYPE} 

# VARS+=-DDO_IO=1
# VARS+=-DFILE_NAME=\"mesh_5k_2k_AOS_stencil1.out\"
VARS+=-DFILE_NAME=\"mesh_5k_2k_SOA_stencil1.out\"
# VARS+=-DFILE_NAME=\"openmp_mesh.out\"
#VARS+=-DUSE_AOS
OPT=-march=native  
#OPT=-march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi

ifdef USE_CALI 
  INC += -I${CALIPER_DIR}/include
  INC += -DUSE_CALI
  LIB += -L${CALIPER_DIR}/lib64 
  LIB += -lcaliper
endif

ifdef USE_LIKWID
  INC += -DUSE_LIKWID
  INC += -I/home/gravelle/soft/src/likwid/intel/include
endif

all: stencil

stencil: stencil.c
	${CC} ${CFLAGS} -o test_stencil ${OPT} ${INC} stencil.c stencil_patterns.c ${LIB} ${VARS}

clean:
	rm -f *.out test_stencil *.o *.cali *.json
	# rm -rf MULTI__*

clena: clean


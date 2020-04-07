# matrix multiplication with openmp intended as a Caliper test

CC=g++-9

INC=
LIB=-fopenmp

USE_CALI=1

X_SIZE=7000
Y_SIZE=5000
# X_SIZE=15
# Y_SIZE=10

# STENCIL_TYPE=0 # 1 deep halo
STENCIL_TYPE=1 # 1 deep cross
# STENCIL_TYPE=2 # 2 deep cross


VARS=-DX_SIZE=${X_SIZE} 
VARS+=-DY_SIZE=${Y_SIZE} 
VARS+=-DSTENCIL_TYPE=${STENCIL_TYPE} 

# VARS+=-DDO_IO=1
# VARS+=-DFILE_NAME=\"mesh_5k_2k_AOS_stencil1.out\"
VARS+=-DFILE_NAME=\"mesh_5k_2k_SOA_stencil1.out\"
# VARS+=-DFILE_NAME=\"openmp_mesh.out\"
# VARS+=-DUSE_AOS


ifdef USE_CALI 
  INC += -I${CALIPER_DIR}/include
  INC += -DUSE_CALI
  LIB += -L${CALIPER_DIR}/lib64 
  LIB += -lcaliper
endif


all: stencil

stencil: stencil.c
	${CC} -g -march=native -o test_stencil ${INC} stencil.c stencil_patterns.c ${LIB} ${VARS}

clean:
	rm -f *.out test_stencil *.o *.cali *.json
	# rm -rf MULTI__*

clena: clean


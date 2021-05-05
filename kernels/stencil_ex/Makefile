# matrix multiplication with openmp intended as a Caliper test

# CC=g++
CC=icc
INC=
LIB=-fopenmp

# USE_CALI=1
# USE_CALI_UNCORE=1
USE_SIMD=1

# CCE=1
# CLANG=1

X_SIZE=14000
Y_SIZE=12000

# X_SIZE=10500
# Y_SIZE=9000

# X_SIZE=3500
# Y_SIZE=3000

# X_SIZE=15
# Y_SIZE=10

# STENCIL_TYPE=0 # 1 deep halo
#STENCIL_TYPE=1 # 1 deep cross
#STENCIL_TYPE=2 # 2 deep cross
# STENCIL_TYPE=3 # 4 deep cross
# STENCIL_TYPE=4 # 4 deep halo
STENCIL_TYPE=5 # 2 deep halo
#

VARS=-DX_SIZE=${X_SIZE} 
VARS+=-DY_SIZE=${Y_SIZE} 
VARS+=-DSTENCIL_TYPE=${STENCIL_TYPE} 

# VARS+=-DDO_IO=1
# VARS+=-DFILE_NAME=\"mesh_5k_2k_AOS_stencil1.out\"
VARS+=-DFILE_NAME=\"mesh_5k_2k_SOA_stencil1.out\"
# VARS+=-DFILE_NAME=\"openmp_mesh.out\"
#VARS+=-DUSE_AOS
INC =
LIB =

MPIRUN = mpirun

# can be individually redined in the  arch settings below
OPT_BASE=-O0 -g 
OMP=-fopenmp
# friendly reminder that -no-fma and -S may be useful -qopt-report-phase=vec

ifdef SKY
ifeq ($(CC),icc)
	OPT_SIMD=-O3 -g -march=skylake-avx512 -qopt-zmm-usage=high
else
	OPT_SIMD=-O3 -g -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
endif
endif

ifdef AMD
	OPT_SIMD=-O3 -g -march=znver2 -mavx2
endif

ifdef A64
ifdef CCE
	CC=cc
	OPT_SIMD=-O3 -g
	OMP=-homp
else
	ifdef CLANG
		CC=armclang
# 		OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record  -msve-vector-bits=512
		OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record
# 		OPT_EXTRA=--target=aarch64-arm-none-eabi
	else
		OPT_SIMD=-O3 -g -march=armv8.2-a+sve -fno-strict-aliasing -fopenmp-simd
	endif
endif
endif

ifdef KNL
ifdef ICC
	OPT_SIMD=-O3 -g -xCOMMON-AVX512
else
	OPT_SIMD=-O3 -g -mavx512f -mavx512cd
endif
endif

ifdef USE_CALI_UNCORE
	INC += -DUSE_CALI_UNCORE
else
	ifdef USE_CALI
		INC += -DUSE_CALI_REG
	endif
endif

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

OPT=${OPT_BASE}
ifdef USE_SIMD
	OPT=${OPT_SIMD}
endif


all: stencil

stencil: stencil.c
	${CC} -o test_stencil ${OPT} ${INC} stencil.c stencil_patterns.c ${LIB} ${VARS}  ${OMP}

clean:
	rm -f *.out test_stencil *.o *.cali *.json
	# rm -rf MULTI__*

clena: clean


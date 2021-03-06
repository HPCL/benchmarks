CC ?= gcc
# choose settings particular to a system
# SKY=1
A64=1
#AMD=1
#KNL=1

# the square matrices are ORDERxORDER
# I recommend having this number be large enough to fill L1 cache
# i.e. 5000*8(bytes per double) = 40K bytes > L1d on Skylake
# 2(rows)*8(bytes per double)*4096 = 64KiB which is equal to L1 on A64

ORDER?=4096
# ORDER?=5120
# ORDER?=5632
# ORDER?=8192
# ORDER?=16384
# ORDER?=20480

# BLOCK_ROWS?=256
# BLOCK_COLS?=256
# BLOCK_ROWS?=8192
# BLOCK_COLS?=8192
BLOCK_ROWS?=256
BLOCK_COLS?=256

# USE_LIKWID=1
# USE_CALI=1
# USE_CALI_UNCORE=1
USE_SIMD=1

INC =
LIB =

MPIRUN = mpirun

# can be individually redined in the  arch settings below
OPT_BASE=-O0 -g 

# friendly reminder that -no-fma and -S may be useful -qopt-report-phase=vec

ifdef SKY
ifeq ($(CC),icc)
	OPT_SIMD=-O3 -g -unroll=0 -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
else
	OPT_SIMD=-O3 -g -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
endif
endif

ifdef AMD
	OPT_SIMD=-O3 -g -march=znver2 -mavx2
endif

ifdef A64
	OPT_SIMD=-O3 -g -march=armv8.2-a+sve -fno-strict-aliasing -fopenmp-simd
endif

ifdef KNL
ifdef ICC
	OPT_SIMD=-O3 -g -xCOMMON-AVX512
else
	OPT_SIMD=-O3 -g -mavx512f -mavx512cd
endif
endif

INC+=-DORDER=${ORDER}
INC+=-DBLOCK_ROWS=${BLOCK_ROWS}
INC+=-DBLOCK_COLS=${BLOCK_COLS}

ifdef USE_CALI 
	INC += -I${CALIPER_DIR}/include
	INC += -DUSE_CALI
	LIB  += -L${CALIPER_DIR}/lib64 
	LIB  += -lcaliper
endif

ifdef USE_CALI_UNCORE
	INC += -DUSE_CALI_UNCORE
else
	ifdef USE_CALI
		INC += -DUSE_CALI_REG
	endif
endif

ifdef USE_LIKWID
	INC += -DLIKWID_PERFMON
	INC += -DUSE_LIKWID
	INC += -I$(LIKWID_DIR)/include
	LIB += -L$(LIKWID_DIR)/lib -llikwid
endif

OPT=${OPT_BASE}
ifdef USE_SIMD
	OPT=${OPT_SIMD}
endif

all: test_linear

test_linear:
	$(CC) -c -o matrix_mul_validated.o ${INC} ${OPT_BASE} matrix_mul_validated.c -fopenmp
	$(CC) -o test_linear.out ${INC} ${OPT} test_linear_algebra.c linear_algebra.c matrix_mul_validated.o ${LIB} -fopenmp

clean:
	rm -f *.log *.o *.out

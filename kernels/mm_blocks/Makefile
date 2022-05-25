CC ?= gcc
# choose settings particular to a system
# SKY=
# CAS=1
A64=1
#AMD=1
#KNL=1

# the square matrices are ORDERxORDER
# I recommend having this number be large enough to fill L1 cache
# i.e. 5000*8(bytes per double) = 40K bytes > L1d on Skylake
# 2(rows)*8(bytes per double)*4096 = 64KiB which is equal to L1 on A64

# ORDER?=4096
# ORDER?=5120
# ORDER?=5632
ORDER?=8192
# ORDER?=16384
# ORDER?=20480

# GAP?=256

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

# CCE=1
# CLANG=1
# FUJ=1

INC =
LIB =

MPIRUN = mpirun

# can be individually redined in the  arch settings below
OPT_BASE=-O0 -g 
OMP=-fopenmp
# friendly reminder that -no-fma and -S may be useful -qopt-report-phase=vec

ifdef SKY
	OMP=-fopenmp
ifeq ($(CC),icc)
	OPT_SIMD=-O3 -g -unroll=0 -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
else
	OPT_SIMD=-O3 -g -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
endif
endif

ifdef CAS
	OMP=-fopenmp
ifeq ($(CC),icc)
	OPT_BASE=-O3 -g -march=cascadelake -no-simd
	OPT_SIMD=-O3 -g -march=cascadelake -qopt-zmm-usage=high
else
	OPT_SIMD=-O3 -g -march=cascadelake 
endif
endif

ifdef AMD
	OPT_SIMD=-O3 -g -march=znver2 -mavx2
endif

ifdef A64
ifdef CCE
	CC=cc
	OPT_SIMD=-O3  -s -hpreferred_vector_width=512
	OMP=-homp
else
	ifdef CLANG
		CC=armclang
		OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record  -msve-vector-bits=512
# 		OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record
# 		OPT_EXTRA=--target=aarch64-arm-none-eabi
	else
		ifdef FUJ
			CC=fcc
			OPT_BASE=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -Kfast -Knosimd -Nlst=t -Koptmsg=2
			OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -Kfast -Ksimd_reg_size=512 -Nlst=t -Koptmsg=2
	# 		OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record
	# 		OPT_EXTRA=--target=aarch64-arm-none-eabi
		else
			OPT_SIMD=-O3 -g -march=armv8.2-a+sve -fno-strict-aliasing -fopenmp-simd -msve-vector-bits=512
		endif
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

INC+=-DORDER=${ORDER}
# INC+=-DGAP=${GAP}
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

ifdef D
OPT += -DDEFAULT
EXE=test_linear_d.out
endif

ifdef I
OPT += -DINTERCHANGE
EXE=test_linear_i.out
endif

ifdef T
OPT += -DTRANSPOSE
EXE=test_linear_t.out
endif

ifdef TG
OPT += -DTRANSPOSE
EXE=test_linear_t_g${GAP}.out
endif

ifdef U
OPT += -DUNROLLJAM
EXE=test_linear_uj.out
endif

ifdef B
OPT += -DBLOCKED
EXE=test_linear_b${BLOCK_ROWS}.out
endif

ifdef BG
OPT += -DBLOCKED
EXE=test_linear_b${BLOCK_ROWS}_g${GAP}.out
endif

ifdef BU
OPT += -DBLOCK_UNROLL
EXE=test_linear_bu${BLOCK_ROWS}.out
endif


ifdef BI
OPT += -DBLOCK_INTERCHANGE
EXE=test_linear_bi${BLOCK_ROWS}.out
endif


all: test_linear

test_linear:
	$(CC) -c -o matrix_mul_validated.o ${INC} ${OPT_BASE} matrix_mul_validated.c ${OMP}
	$(CC) -c ${INC} ${OPT} linear_algebra.c ${OMP}
	$(CC) -c ${INC} ${OPT} test_linear_algebra.c ${OMP}
	$(CC) -o ${EXE} ${INC} ${OPT} test_linear_algebra.o linear_algebra.o matrix_mul_validated.o ${LIB} ${OMP}
# 	$(CC) -o test_linear_b${BLOCK_ROWS}.out ${INC} ${OPT} test_linear_algebra.o linear_algebra.o matrix_mul_validated.o ${LIB} ${OMP}
# 	$(CC) -o test_linear_uj.out ${INC} ${OPT} test_linear_algebra.o linear_algebra.o matrix_mul_validated.o ${LIB} ${OMP}

assembly:
	$(CC) -c ${INC} ${OPT} -S linear_algebra_separate.c ${OMP}

clean:
	rm -f *.log *.o *.out

CC = gcc
# choose settings particular to a system
# SKY=1
A64=1
#AMD=1
#KNL=1

# the square matrices are ORDERxORDER
# I recommend having this number be large enough to fill L1 cache
# i.e. 5000*8(bytes per double) = 40K bytes > L1d on Skylake
# 2(rows)*8(bytes per double)*4096 = 64KiB which is equal to L1 on A64
ifndef ORDER
# 	ORDER=4096
# 	ORDER=5120
# 	ORDER=5632
	ORDER=8192
# 	ORDER=16384
# 	ORDER=20480
endif

# USE_CALI=1
# USE_SIMD=1

INC =
LIB =

MPIRUN = mpirun

# can be individually redined in the  arch settings below
OPT_BASE=-O0 -g

ifdef SKY
OPT_SIMD=-O3 -g  -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
endif

ifdef AMD
OPT_SIMD=-O3 -g -march=znver2 -mavx2
endif

ifdef A64
OPT_SIMD=-O3 -g -march=armv8.2-a+sve  -fopenmp-simd
endif

ifdef KNL
ifdef ICC
OPT_SIMD=-O3 -g -xCOMMON-AVX512
else
OPT_SIMD=-O3 -g -mavx512f -mavx512cd
endif
endif


INC+=-DORDER=${ORDER}



ifdef USE_CALI 
  INC += -I${CALIPER_DIR}/include
  INC += -DUSE_CALI
  LIB  += -L${CALIPER_DIR}/lib64 
  LIB  += -lcaliper
endif

OPT=${OPT_BASE}
ifdef USE_SIMD
OPT=${OPT_SIMD}
endif

all: test_linear

test_linear:
		$(CC) -o test_linear.out ${INC} ${OPT} test_linear_algebra.c linear_algebra.c ${LIB} -fopenmp

clean:
		rm -f *.log *.o *.out
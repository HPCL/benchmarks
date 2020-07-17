# micro-benchmark to validate counters used in the Top-Down Method 

# Settings; change these to select the system and optimization levels

# choose settings particular to a system
SKY=1
#ARM=1
#AMD=1
#KNL=1


CC=gcc
ifdef INTEL_LICENSE_FILE
CC=icc
ICC=1
endif

# USE_CALI=1
USE_SIMD=1

INC =
LIB =


# can be individually redined in the  arch settings below
OPT_BASE=-O0

ifdef SKY
OPT_SIMD=-O3  -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
endif

ifdef AMD
OPT_SIMD=-O3 -march=znver2 -mavx2
endif

ifdef ARM
OPT_SIMD=-O3 -march=armv8.1-a+simd
endif

ifdef KNL
ifdef ICC
OPT_SIMD=-O3 -xCOMMON-AVX512
else
OPT_SIMD=-O3 -mavx512f -mavx512cd
endif
endif

OPT=${OPT_BASE}
ifdef USE_SIMD
OPT=${OPT_SIMD}
endif

# INC+=-DORDER=${ORDER}



ifdef USE_CALI 
  INC += -I${CALIPER_DIR}/include
  INC += -DUSE_CALI
  LIB  += -L${CALIPER_DIR}/lib64 
#  LIB  += -lcaliper -lcaliper-mpi
  LIB  += -lcaliper
endif


# OPT += -qopt-streaming-stores=never

all: measure_bounds

measure_bounds: measure_bounds.c
	${CC} ${OPT} -g -std=c11 -o measure_bounds ${INC} measure_bounds.c ${LIB} -fopenmp	

assembly: measure_bounds.c
	${CC} ${OPT} -g -std=c11 -S -o measure_bounds ${INC} measure_bounds.c ${LIB} -fopenmp	

.PHONY: test
test: measure_bounds
	./measure_bounds

.PHONY: clean
clean:
	$(RM) measure_bounds *.o
	# rm -rf MULTI__*

clena: clean


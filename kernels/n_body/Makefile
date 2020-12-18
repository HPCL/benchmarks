# micro-benchmark to validate counters used in the Top-Down Method 

# Settings; change these to select the system and optimization levels

# choose settings particular to a system
# SKY=1
#TX2=1
A64=1
#AMD=1
#KNL=1


CC=gcc
ifdef INTEL_LICENSE_FILE
CC=icc
ICC=1
endif

USE_CALI=1
USE_SIMD=1

INC =
LIB =

# can be individually redined in the  arch settings below
OPT_BASE=-O0

ifdef SKY
OPT_SIMD=-O3 -DVLEN=8 -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi
endif

ifdef AMD
OPT_SIMD=-O3 -DVLEN=4 -march=znver2 -mavx2
endif

ifdef TX2
OPT_SIMD=-O3 -DVLEN=2 -march=armv8.1-a+simd
endif

ifdef A64
OPT_SIMD=-O3 -DVLEN=2 -march=armv8.2-a+sve
endif

ifdef KNL
ifdef ICC
OPT_SIMD=-O3 -DVLEN=8 -xCOMMON-AVX512
else
OPT_SIMD=-O3 -DVLEN=8 -mavx512f -mavx512cd
endif
endif

OPT=${OPT_BASE}
ifdef USE_SIMD
OPT=${OPT_SIMD}
OPT += -DUSE_SIMD
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

all: n_body

n_body: n_body.c
	${CC} ${OPT} -g -std=c11 -o n_body ${INC} n_body.c ${LIB} -fopenmp -lm 

.PHONY: test

test: n_body
	./n_body

.PHONY: clean

clean:
	$(RM) n_body *.o

clena: clean


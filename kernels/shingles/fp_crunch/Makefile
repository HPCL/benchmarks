# micro-benchmark to validate counters used in the Top-Down Method 

# Settings; change these to select the system and optimization levels

# choose settings particular to a system
CAS=1
# SKY=1
#TX2=1
# A64=1
#AMD=1
#KNL=1


CC=gcc
ifdef INTEL_LICENSE_FILE
CC=icc
ICC=1
endif

USE_CALI=1
USE_CALI_UNCORE=1
USE_SIMD=1

INC =
LIB =

# can be individually redined in the  arch settings below
OPT_BASE=-O0

ifdef SKY
  OMP=-fopenmp
ifeq ($(CC),icc)
  OPT_SIMD=-O3 -g -DVLEN=8 -march=skylake-avx512 -qopt-zmm-usage=high
else
  OPT_SIMD=-O3 -g -DVLEN=8 -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
endif
endif

ifdef CAS
  OMP=-fopenmp
ifeq ($(CC),icc)
  OPT_BASE=-O1 -g -march=cascadelake -no-simd
  OPT_SIMD=-O2 -g -DVLEN=8 -march=cascadelake -qopt-zmm-usage=high
else
  OPT_SIMD=-O3 -g -DVLEN=8 -march=cascadelake 
endif
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


DEFS = -DDATA_SIZE=1024
ifdef DATA_SIZE
DEFS = -DDATA_SIZE=${DATA_SIZE}
endif

ifdef SP
DEFS += -DSP
else
DEFS += -DDP
endif


INC += ${DEFS}

# INC+=-DORDER=${ORDER}



ifdef USE_CALI 
  INC += -I${CALIPER_DIR}/include
  INC += -DUSE_CALI
  LIB  += -L${CALIPER_DIR}/lib64 
#  LIB  += -lcaliper -lcaliper-mpi
  LIB  += -lcaliper
endif

ifdef USE_CALI_UNCORE
  INC += -DUSE_CALI_UNCORE
else
  ifdef USE_CALI
    INC += -DUSE_CALI_REG
  endif
endif

# OPT += -qopt-streaming-stores=never

all: fp_crunch

fp_crunch: fp_crunch.c fma.c Makefile
	${CC} ${OPT} -g -c -std=c11 ${INC} fma.c ${LIB} 
	${CC} ${OPT_BASE} -g -std=c11 -o fp_crunch ${INC} fp_crunch.c fma.o ${LIB} -fopenmp -fopenmp-simd -lm 

.PHONY: test

test: fp_crunch
	./fp_crunch

.PHONY: clean

clean:
	$(RM) fp_crunch *.o

clena: clean


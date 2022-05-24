# micro-benchmark to validate counters used in the Top-Down Method 

# Settings; change these to select the system and optimization levels

# choose settings particular to a system
# SKY=1
# A64=1
# ARM=1
#AMD=1
#KNL=1
CAS=1

CC=gcc
ifdef INTEL_LICENSE_FILE
CC=icc
ICC=1
endif

USE_CALI=1
USE_SIMD=1
# CLANG=1

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

OMP=-fopenmp

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


ifdef CAS
  OMP=-fopenmp
ifeq ($(CC),icc)
  OMP=-qopenmp
  OPT_BASE=-O1 -g -march=cascadelake -no-simd
  OPT_SIMD=-O2 -g -march=cascadelake -qopt-zmm-usage=high
else
  OPT_SIMD=-O3 -g -march=cascadelake 
endif
endif

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


all: miss_count cache_conflict byte_validation

miss_count: miss_count.c
	${CC} ${OPT} -std=c11 -g -o miss_count ${INC} miss_count.c ${LIB} -fopenmp	


cache_conflict: cache_conflict.c
	${CC} ${OPT} -std=c11 -g -o cache_conflict ${INC} cache_conflict.c ${LIB} ${OMP}

byte_validation: byte_validation.c
	${CC} ${OPT} -std=c11 -g -o byte_validation ${INC} byte_validation.c ${LIB} -fopenmp	

.PHONY: test
test: miss_count
	./miss_count

.PHONY: clean
clean:
	$(RM) miss_count cache_conflict byte_validation *.o
	# rm -rf MULTI__*

clena: clean


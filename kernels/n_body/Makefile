# micro-benchmark to validate counters used in the Top-Down Method 

# Settings; change these to select the system and optimization levels

# choose settings particular to a system
# CAS=1
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

GAP?=0

# USE_CALI=1
# USE_CALI_UNCORE=1
USE_SIMD=1

INC =
LIB =

# CCE=1
#CLANG=1
FUJ=1

# can be individually redined in the  arch settings below
OPT_BASE=-O0 
OMP=-fopenmp

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
  OPT_SIMD=-O3 -g -DVLEN=8 -march=cascadelake -qopt-zmm-usage=high
else
  OPT_SIMD=-O3 -g -DVLEN=8 -march=cascadelake 
endif
endif

ifdef AMD
OPT_SIMD=-O3 -DVLEN=4 -march=znver2 -mavx2 -fopenmp
endif

ifdef TX2
OPT_SIMD=-O3 -DVLEN=2 -march=armv8.1-a+simd -fopenmp
endif

ifdef A64
ifdef CCE
  CC=cc
  OPT_SIMD=-O3  -s -hpreferred_vector_width=512 -fopenmp
  OMP=-homp
else
  ifdef CLANG
    CC=armclang
    OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record  -msve-vector-bits=512
    OMP=-fopenmp
#     OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record
#     OPT_EXTRA=--target=aarch64-arm-none-eabi
  else
    ifdef FUJ
      CC=fcc
      OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -Kfast,openmp,ocl -Nlst=t -Koptmsg=2 -Ksimd_reg_size=512 -fsave-optimization-record
      OMP=  
  #     OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record
  #     OPT_EXTRA=--target=aarch64-arm-none-eabi
    else
      OPT_SIMD=-O3 -g -march=armv8.2-a+sve -fno-strict-aliasing -fopenmp-simd -msve-vector-bits=512
      OMP=-fopenmp
    endif
  endif
endif
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

ifdef USE_CALI_UNCORE
  INC += -DUSE_CALI_UNCORE
else
  ifdef USE_CALI
    INC += -DUSE_CALI_REG
  endif
endif

INC+=-DGAP=${GAP}

# OPT += -qopt-streaming-stores=never

all: n_body

n_body:
	${CC} ${OPT} -g -std=c11 -o n_body ${INC} n_body.c ${LIB} -lm 

.PHONY: test

test: n_body
	./n_body

.PHONY: clean

clean:
	$(RM) n_body *.o

clena: clean


# micro-benchmark to validate counters used in the Top-Down Method 

# Settings; change these to select the system and optimization levels

# choose settings particular to a system
# CAS=1
# SKY=1
#TX2=1
A64=1
#AMD=1
#KNL=1

DATA_SIZE?=1024

CC?=gcc
ifeq ($(CC),icc)
CC=icc
ICC=1
endif

# CCE=1
# CLANG=1
# FUJ=1

USE_CALI=1
# USE_CALI_UNCORE=1
USE_SIMD=1
# USE_STRICT=1

INC =
LIB =

# can be individually redined in the  arch settings below
OPT_BASE=-O0 -std=c11 
OMP=-fopenmp

ifdef SKY
  OMP=-fopenmp -fopenmp-simd
ifeq ($(CC),icc)
  OMP=-qopenmp -qopenmp-simd
  OPT_SIMD=-O2 -g -march=skylake-avx512 -qopt-zmm-usage=high
else
  OPT_SIMD=-O3 -g -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
endif
endif

ifdef CAS
  OMP=-fopenmp -fopenmp-simd
ifeq ($(CC),icc)
  OMP=-qopenmp -qopenmp-simd
  #OPT_BASE=-O1 -g -march=cascadelake -no-simd
  OPT_BASE=-O2 -std=c11  -g -march=cascadelake  
  OPT_SIMD=-O3 -std=c11  -g -march=cascadelake -qopt-zmm-usage=high  -qopt-report=5 -qopt-report-phase=vec 
#   OPT_SIMD=-O2 -g -march=cascadelake -qopt-zmm-usage=high -fno-inline-functions
  ifdef USE_STRICT
    OPT_BASE+=-fp-model=precise
    OPT_SIMD+=-fp-model=precise
  endif
else
  OPT_BASE=-O2 -std=c11  -g -march=cascadelake
  OPT_SIMD=-O2 -std=c11  -g -march=cascadelake
endif

endif

ifdef AMD
OPT_SIMD=-O3 -std=c11  -march=znver2 -mavx2
endif

ifdef TX2
OPT_SIMD=-O3 -std=c11  -march=armv8.1-a+simd
endif

ifdef A64
ifdef CCE
  CC=cc
  OPT_SIMD=-O3 -s -hpreferred_vector_width=512 -h list=admi
  OMP=-homp
else
  ifdef CLANG
    CC=armclang
    OPT_SIMD=-O3 -std=c11  -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record  -msve-vector-bits=512 -ffp-contract=fast -fstrict-aliasing
    OMP=-fopenmp -fopenmp-simd
#     OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record
#     OPT_EXTRA=--target=aarch64-arm-none-eabi
  else
    ifdef FUJ
      CC=fcc
      OPT_BASE=-O3 -std=c11  -g -march=armv8.2-a+sve -mcpu=a64fx -Kfast -Knosimd -Nlst=t -Koptmsg=2
      OPT_SIMD=-O3 -std=c11  -g -march=armv8.2-a+sve -mcpu=a64fx -Kfast -Ksimd_reg_size=512 -Nlst=t -Koptmsg=2
  #     OPT_SIMD=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -fsave-optimization-record
  #     OPT_EXTRA=--target=aarch64-arm-none-eabi
    else
#       OPT_SIMD=-Ofast -std=c11  -g -march=armv8.2-a+sve -fstrict-aliasing -fopenmp-simd -msve-vector-bits=512 -fvect-cost-model=unlimited -fopt-info
      OPT_SIMD=-O3 -std=c11  -g -march=armv8.2-a+sve -fstrict-aliasing -fopenmp-simd -msve-vector-bits=512 -fvect-cost-model=unlimited 
      OPT_SIMD+=-fopt-info -fallow-store-data-races 
      #fast-math
      OPT_SIMD+=-fno-math-errno -ffinite-math-only -fno-rounding-math -fno-signaling-nans -fcx-limited-range -fexcess-precision=fast 
      #-funsafe-math-optimizations
      OPT_SIMD+=-fno-signed-zeros 
      OPT_SIMD+=-fno-trapping-math 
      OPT_SIMD+=-fassociative-math 
      OPT_SIMD+= -freciprocal-math
    endif
  endif
endif
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

# all: default tuned
all: default cos log div add

default: fp_crunch.c fma.c _tune_fma.c Makefile
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -g -c ${INC} fma.c ${LIB}  ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -g -c ${INC} fp_utils.c ${LIB} ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -g -o fp_crunch_default ${INC} fp_crunch.c fp_utils.o fma.o ${LIB} ${OMP} -lm

cos: fp_crunch.c fma.c _tune_fma.c Makefile
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_COS=1 -g -c ${INC} fma.c ${LIB}  ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_COS=1 -g -c ${INC} fp_utils.c ${LIB} ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_COS=1 -g -o fp_crunch_cos ${INC} fp_crunch.c fp_utils.o fma.o ${LIB} ${OMP} -lm

log: fp_crunch.c fma.c _tune_fma.c Makefile
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_LOG=1 -g -c ${INC} fma.c ${LIB}  ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_LOG=1 -g -c ${INC} fp_utils.c ${LIB} ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_LOG=1 -g -o fp_crunch_log ${INC} fp_crunch.c fp_utils.o fma.o ${LIB} ${OMP} -lm

div: fp_crunch.c fma.c _tune_fma.c Makefile
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_DIV=1 -g -c ${INC} fma.c ${LIB}  ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_DIV=1 -g -c ${INC} fp_utils.c ${LIB} ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_DIV=1 -g -o fp_crunch_div ${INC} fp_crunch.c fp_utils.o fma.o ${LIB} ${OMP} -lm

add: fp_crunch.c fma.c _tune_fma.c Makefile
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_ADD=1 -g -c ${INC} fma.c ${LIB}  ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_ADD=1 -g -c ${INC} fp_utils.c ${LIB} ${OMP}
	${CC} ${OPT} -DTUNED -DDATA_SIZE=${DATA_SIZE} -DUSE_ADD=1 -g -o fp_crunch_add ${INC} fp_crunch.c fp_utils.o fma.o ${LIB} ${OMP} -lm

# tuned: fp_crunch.c fma.c _tune_fma.c Makefile
#   ${CC} ${OPT} -DDATA_SIZE=${DATA_SIZE} -g -c ${INC} _tune_fma.c ${LIB}  ${OMP} -o fma.o
#   ${CC} ${OPT} -DDATA_SIZE=${DATA_SIZE} -g -c ${INC} fp_utils.c ${LIB} ${OMP}
#   ${CC} ${OPT} -DDATA_SIZE=${DATA_SIZE} -g -o fp_crunch_tuned ${INC} fp_crunch.c fp_utils.o fma.o ${LIB} ${OMP} -lm 

tuned: fp_crunch.c fma.c  Makefile
	${CC} ${OPT} -DDATA_SIZE=${DATA_SIZE} -DTUNED -g -c ${INC} fma.c ${LIB}  ${OMP}
	${CC} ${OPT} -DDATA_SIZE=${DATA_SIZE} -DTUNED -g -c ${INC} fp_utils.c ${LIB} ${OMP}
	${CC} ${OPT} -DDATA_SIZE=${DATA_SIZE} -DTUNED -g -o fp_crunch_tuned ${INC} fp_crunch.c fp_utils.o fma.o ${LIB} ${OMP} -lm

.PHONY: test

test: fp_crunch
	./fp_crunch

.PHONY: clean

clean:
	$(RM) fp_crunch *.o

clena: clean


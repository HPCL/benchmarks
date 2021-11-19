
CFLAGS = -O2 -fopenmp -g -qopt-streaming-stores=auto
#CFLAGS += -S 
LIB = 

CAS=1
# SKY=1
#TX2=1
# A64=1
#AMD=1
#KNL=1

CC?=gcc
ifeq ($(CC),icc)
CC=icc
ICC=1
endif

# USE_CALI=1
# USE_CALI_UNCORE=1
USE_SIMD=1

INC =
LIB =

# can be individually redined in the  arch settings below
OPT_BASE=-O0

OMP=-fopenmp

ifdef SKY
  OMP=-qopenmp
ifeq ($(CC),icc)
  OPT_SIMD=-O3 -g -march=skylake-avx512 -qopt-zmm-usage=high
else
  OPT_SIMD=-O3 -g -march=skylake-avx512 -mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl -mavx512ifma -mavx512vbmi 
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

ifdef AMD
OMP=-qopenmp
OPT_SIMD=-O3 -DVLEN=4 -march=znver2 -mavx2
endif

ifdef TX2
OMP=-qopenmp
OPT_SIMD=-O3 -DVLEN=2 -march=armv8.1-a+simd
endif

ifdef A64
OMP=-qopenmp
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


##################################################################################
# Build stream with GNU compiler.  Set NTIMES=10 to scan over a large number
# of cases.  Set NTIMES=500 to run a small number of cases for the best result.
# L1:  Set STREAM_ARRAY_SIZE = 196608     = 2^16 * 3.
# L2:  Set STREAM_ARRAY_SIZE = 2097152    = 2^21.
# DDR: Set STREAM_ARRAY_SIZE = 1871708160 = 2^20 * 3   * 5 * 7 * 17.
#---------------------------------------------------------------------------------
# DDR: Set STREAM_ARRAY_SIZE = 2246049792 = 2^21 * 3^2     * 7 * 17.

#INC+=" -DSTREAM_ARRAY_SIZE=2246049792 -DNTIMES=10 -DNTIMES_KK=15"
#---------------------------------------------------------------------------------
INC+=-DSTREAM_ARRAY_SIZE=196608     -DNTIMES=10 -DNTIMES_KK=171360
# INC+=-DSTREAM_ARRAY_SIZE=2097152    -DNTIMES=10 -DNTIMES_KK=16065
# INC+=-DSTREAM_ARRAY_SIZE=1871708160 -DNTIMES=10 -DNTIMES_KK=18


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


ifdef USE_LIKWID
  CFLAGS += -DUSE_LIKWID
  CFLAGS += -I/home/gravelle/soft/src/likwid/intel/include
endif

STREAM_C_FLAGS="-Ofast"
STREAM_C_FLAGS+=" -fopenmp"
STREAM_C_FLAGS+=" -ffast-math -fomit-frame-pointer"
STREAM_C_FLAGS+=" -fno-strict-aliasing"
STREAM_C_FLAGS+=" -fno-unsafe-math-optimizations"
STREAM_C_FLAGS+=" -mcpu=a64fx"
STREAM_C_FLAGS+=" -msve-vector-bits=512"

all: stream_c.exe
# all: stream_f.exe stream_c.exe

# 800000000 should be >4x apollo's L3
stream_c.exe: stream.c
	$(CC) -g $(OPT) $(INC) stream.c -mcmodel=large -DTUNED=1 -DTRIAD=1 $(LIB) -o stream_c.exe $(OMP)

clean:
	rm -f stream_f.exe stream_c.exe *.o
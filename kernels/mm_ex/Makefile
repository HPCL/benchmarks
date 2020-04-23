# matrix multiplication with mpi intended as a Caliper test

# Settings; change these to select the system and optimization levels

# choose settings particular to a system
SKY=1
#ARM=1
#AMD=1
#KNL=1

# the square matrices are ORDERxORDER
# I recommend having this number be large enough to fill L1 cache
# i.e. 5000*8(bytes per double) = 40K bytes > L1d on Skylake
ifndef ORDER
        ORDER=5000
endif


ifdef INTEL_LICENSE_FILE
ICC=1
endif

#USE_CALI=1


MPICC = mpicc
INC =
LIB =

MPIRUN = mpirun

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


INC+=-DORDER=${ORDER}



ifdef USE_CALI 
  INC += -I${CALIPER_DIR}/include
  INC += -DUSE_CALI
  LIB  += -L${CALIPER_DIR}/lib64 
  LIB  += -lcaliper -lcaliper-mpi
endif


all: mm_mpi

mm_mpi: mm_mpi.c
	${MPICC} ${OPT} -g -o mm_mpi ${INC} mm_mpi.c ${LIB} -fopenmp	

.PHONY: test
test: mm_mpi
	$(MPIRUN) -np 4 mm_mpi

.PHONY: clean
clean:
	$(RM) mm_mpi *.o
	# rm -rf MULTI__*

clena: clean


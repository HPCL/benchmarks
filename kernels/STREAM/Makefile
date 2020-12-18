CC = gcc
CC = icc
CFLAGS = -O2 -fopenmp -g -qopt-streaming-stores=auto
#CFLAGS += -S 
LIB = 

USE_LIKWID=1
#USE_CALI=1

ifdef USE_CALI
CFLAGS += -I${CALIPER_DIR}/include
CFLAGS += -DUSE_CALI
LIB  += -L${CALIPER_DIR}/lib64 
LIB  += -lcaliper 
endif


ifdef USE_LIKWID
  CFLAGS += -DUSE_LIKWID
  CFLAGS += -I/home/gravelle/soft/src/likwid/intel/include
endif


FC = gfortran-4.9
FFLAGS = -O2 -fopenmp

all: stream_c.exe
# all: stream_f.exe stream_c.exe

stream_f.exe: stream.f mysecond.o
	$(CC) $(CFLAGS) -c mysecond.c
	$(FC) $(FFLAGS) -c stream.f
	$(FC) $(FFLAGS) stream.o mysecond.o -o stream_f.exe

# 800000000 should be >4x apollo's L3
stream_c.exe: stream.c
	$(CC) $(CFLAGS) stream.c -mcmodel=large -DSTREAM_ARRAY_SIZE=800000000 -DTUNED=1 $(LIB) -o stream_c.exe

clean:
	rm -f stream_f.exe stream_c.exe *.o

# an example of a more complex build line for the Intel icc compiler
stream.icc: stream.c
	icc -O3 -xCORE-AVX2 -ffreestanding -qopenmp -DSTREAM_ARRAY_SIZE=80000000 -DNTIMES=20 stream.c -o stream.omp.AVX2.80M.20x.icc

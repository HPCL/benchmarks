#===============================================================================
# User Options
#===============================================================================

# Compiler can be set below, or via environment variable
CC ?= gcc
ifeq ($(CC),)
CC    = gcc
endif
OPTIMIZE = yes
DEBUG = yes
#PROFILE = yes
#MPI = yes
# USE_CALI = yes
# USE_CALI_UNCORE = yes
OPENMP = yes

#===============================================================================
# Program name & source code list
#===============================================================================

program = XSBench

source = \
Main.c \
io.c \
Simulation.c \
GridInit.c \
XSutils.c \
Materials.c

obj = $(source:.c=.o)

#===============================================================================
# Sets Flags
#===============================================================================

# Make sure we didn't pick up lower case cc
ifeq (cc,$(CC))
CC    = gcc
#CC    = cc # for cray compiler
endif

# Standard Flags
CFLAGS := -std=gnu99 -Wall

# Linker Flags
LDFLAGS = -lm

# Regular gcc Compiler
ifneq (,$(findstring gcc,$(CC)))
  CFLAGS += -flto
  ifeq ($(OPENMP),yes)
	CFLAGS += -fopenmp -DOPENMP
  endif
endif

# LLVM Compiler
ifneq (,$(findstring clang,$(CC)))
  CFLAGS += -flto
  ifeq ($(OPENMP),yes)
	CFLAGS += -fopenmp -DOPENMP
  endif
endif

# Intel Compiler
ifneq (,$(findstring icc,$(CC)))
  CFLAGS += -ipo
  CFLAGS=-O1 -g -march=cascadelake -no-simd
  ifeq ($(OPTIMIZE),yes)
  CFLAGS=-O3 -g -DVLEN=8 -march=cascadelake -qopt-zmm-usage=high
  endif
  ifeq ($(OPENMP),yes)
  CFLAGS += -fopenmp -DOPENMP
  endif
endif

# Fujitsu Compiler
ifneq (,$(findstring fcc,$(CC)))
  CFLAGS=-O3 -g -march=armv8.2-a -mcpu=a64fx -Kfast -Knosimd
  ifeq ($(OPTIMIZE),yes)
  CFLAGS=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -Kfast -Ksimd_reg_size=512
  endif
  ifeq ($(OPENMP),yes)
  CFLAGS += -fopenmp -DOPENMP
  endif
endif

# Debug Flags
ifeq ($(DEBUG),yes)
  #CFLAGS += -g -Winline
  CFLAGS += -g
  LDFLAGS += -g
endif

ifeq ($(USE_CALI),yes)
  CFLAGS  += -g
  LDFLAGS += -g
  CFLAGS  += -I${CALI_DIR}/include
  CFLAGS  += -DUSE_CALI
  LDFLAGS += -L${CALI_DIR}/lib64
  LDFLAGS += -lcaliper

  ifdef USE_CALI_UNCORE
    CFLAGS += -DUSE_CALI_UNCORE
  else
    ifdef USE_CALI
      CFLAGS += -DUSE_CALI_REG
    endif
  endif

endif

# Profiling Flags
ifeq ($(PROFILE),yes)
  CFLAGS += -pg
  LDFLAGS  += -pg
endif

# Optimization Flags
ifeq ($(OPTIMIZE),yes)
  #CFLAGS += -O3 -mcpu=thunderx2t99 -mcmodel=large
  #CFLAGS=-O3 -g -march=armv8.2-a+sve -mcpu=a64fx -Kfast -Ksimd_reg_size=512 -DOPENMP
  #CFLAGS += -O3 -mcmodel=large
  #CFLAGS += -O3 -march=armv8.2-a+sve -mcpu=native -mcmodel=large
endif

# MPI
ifeq ($(MPI),yes)
  CC = mpicc
  CFLAGS += -DMPI
endif


#===============================================================================
# Targets to Build
#===============================================================================

$(program): $(obj) XSbench_header.h Makefile
	$(CC) $(CFLAGS) $(obj) -o $@ $(LDFLAGS)

%.o: %.c XSbench_header.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(program) $(obj) core.*

edit:
	vim -p $(source) XSbench_header.h

run:
	./$(program)

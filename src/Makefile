# Cirrus
#CXX = clang++
#CXXFLAGS = -stdlib=libc++ -fopenmp -fopenmp-offload-mandatory -fopenmp-targets=x86_64 -Xopenmp-target=x86_64 --offload-arch=x86_64 -g
#PYTHON_DIR = /work/z04/shared/quantum-gpu/miniconda3/envs/qiskit
#NLOPT_DIR = /work/z04/z04/jlee7-qu/llvm-quantum-omp/OpenMP-Quantum/nlopt/install
#LDFLAGS = -lpython3.11 -lnlopt -L$(PYTHON_DIR)/lib -L$(NLOPT_DIR)/lib64
#LOCALLIB_FLAG = LIBRARY_PATH=$(PYTHON_DIR)/lib:$(NLOPT_DIR)/lib64

# Local
CC = gcc-13
CFLAGS = -g -O3 -fPIC
PYTHON_DIR = /Users/jlee7/miniconda3/envs/omp_q
NLOPT_DIR = /Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/nlopt/install
RABBITMQ_DIR=/Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/rabbitmq/install
JANSSON_DIR=/Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/jansson/install
LDFLAGS = -shared -ljansson -lrabbitmq -lpython3.11 -lnlopt -L$(PYTHON_DIR)/lib -L$(NLOPT_DIR)/lib -L$(RABBITMQ_DIR)/lib -L$(JANSSON_DIR)/lib
LOCALLIB_FLAG = LIBRARY_PATH=$(RABBITMQ_DIR)/lib:$(JANSSON_DIR)/lib:$(PYTHON_DIR)/lib:$(NLOPT_DIR)/lib


INCLUDES = -I$(JANSSON_DIR)/include -I$(RABBITMQ_DIR)/include -I$(PYTHON_DIR)/include/python3.11 -I$(NLOPT_DIR)/include -I../include


SRC_DIR = /Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/
BUILD_DIR = $(SRC_DIR)/build



# Source files
SRC_FILES := $(wildcard *.c)

# Object files (automatically generated from source files)
OBJ_FILES := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Target
all: $(BUILD_DIR)/libOMPQ.dylib


$(BUILD_DIR)/libOMPQ.dylib: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(LOCALLIB_FLAG) $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
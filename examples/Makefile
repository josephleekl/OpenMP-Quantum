# Cirrus
#CXX = clang++
#CXXFLAGS = -stdlib=libc++ -fopenmp -fopenmp-offload-mandatory -fopenmp-targets=x86_64 -Xopenmp-target=x86_64 --offload-arch=x86_64 -g
#PYTHON_DIR = /work/z04/shared/quantum-gpu/miniconda3/envs/qiskit
#NLOPT_DIR = /work/z04/z04/jlee7-qu/llvm-quantum-omp/OpenMP-Quantum/nlopt/install
#LDFLAGS = -lpython3.11 -lnlopt -L$(PYTHON_DIR)/lib -L$(NLOPT_DIR)/lib64
#LOCALLIB_FLAG = LIBRARY_PATH=$(PYTHON_DIR)/lib:$(NLOPT_DIR)/lib64

# Local
CXX = g++-13
CXXFLAGS = -g -O3

SRC_DIR = /Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/
BUILD_DIR = $(SRC_DIR)/build

PYTHON_DIR = /Users/jlee7/miniconda3/envs/omp_q
NLOPT_DIR = /Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/nlopt/install
RABBITMQ_DIR=/Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/rabbitmq/install
JANSSON_DIR=/Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/jansson/install
LDFLAGS = -ljansson -lrabbitmq -lpython3.11 -lnlopt -L$(PYTHON_DIR)/lib -L$(NLOPT_DIR)/lib -L$(RABBITMQ_DIR)/lib -L$(JANSSON_DIR)/lib
LOCALLIB_FLAG = LIBRARY_PATH=$(RABBITMQ_DIR)/lib:$(JANSSON_DIR)/lib:$(PYTHON_DIR)/lib:$(NLOPT_DIR)/lib:$(BUILD_DIR)

INCLUDES = -I$(JANSSON_DIR)/include -I$(RABBITMQ_DIR)/include -I$(PYTHON_DIR)/include/python3.11 -I$(NLOPT_DIR)/include -I../include


# Directories


# Example files
EXAMPLE_FILES = $(wildcard *.cpp)

# Target
all: $(EXAMPLE_FILES:%.cpp=$(BUILD_DIR)/%)

$(BUILD_DIR)/%: %.cpp
	@mkdir -p $(@D)
	$(LOCALLIB_FLAG) $(CXX) $(CXXFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $<  -lOMPQ

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

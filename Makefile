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
PYTHON_DIR = /Users/jlee7/miniconda3/envs/omp_q
NLOPT_DIR = /Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/nlopt/install
LDFLAGS = -lpython3.11 -lnlopt -L$(PYTHON_DIR)/lib -L$(NLOPT_DIR)/lib
LOCALLIB_FLAG = LIBRARY_PATH=$(PYTHON_DIR)/lib:$(NLOPT_DIR)/lib



INCLUDES = -I$(PYTHON_DIR)/include/python3.11 -I$(NLOPT_DIR)/include

# Source files and output executables
SRCS = vqe-offload.cpp qaoa-offload.cpp
OBJS = $(SRCS:.cpp=.o) 
EXES = $(SRCS:.cpp=)  

# Header file
HDR = omp_q.h

# Default target
all: $(EXES)

# Compile the source files into object files
%.o: %.cpp $(HDR)
	$(LOCALLIB_FLAG) $(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Link object files into executables
$(EXES): %: %.o
	$(LOCALLIB_FLAG) $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< 

# Clean up the compiled files
clean:
	rm -f $(OBJS) $(EXES)
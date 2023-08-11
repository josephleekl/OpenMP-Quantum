# Cirrus
#CXX = clang++
#CXXFLAGS = -stdlib=libc++ -fopenmp -fopenmp-offload-mandatory -fopenmp-targets=x86_64 -Xopenmp-target=x86_64 --offload-arch=x86_64 
#LDFLAGS = -lpython3.11 -L/work/z04/shared/quantum-gpu/miniconda3/envs/qiskit/lib 
#INCLUDES = -I/work/z04/shared/quantum-gpu/miniconda3/envs/qiskit/include/python3.11

# Local
LOCALLIB_FLAG = LIBRARY_PATH=/Users/jlee7/miniconda3/envs/omp_q/lib:/Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/nlopt/install/lib
CXX = g++-13
INCLUDES = -I/Users/jlee7/miniconda3/envs/omp_q/include/python3.11 -I/Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/nlopt/install/include
LDFLAGS = -lpython3.11 -lnlopt -L/Users/jlee7/miniconda3/envs/omp_q/lib -L/Users/jlee7/Desktop/EPCC/projects/quantum/openmp/OpenMP-Quantum/nlopt/install/lib




# Source files and output executables
SRCS = vqe.cpp
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
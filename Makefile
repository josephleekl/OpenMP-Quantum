CXX = clang++
CXXFLAGS = -stdlib=libc++ -fopenmp -fopenmp-offload-mandatory -fopenmp-targets=x86_64 -Xopenmp-target=x86_64 --offload-arch=x86_64 
LDFLAGS = -lpython3.11 -L/work/z04/shared/quantum-gpu/miniconda3/envs/qiskit/lib 
INCLUDES = -I/work/z04/shared/quantum-gpu/miniconda3/envs/qiskit/include/python3.11


# Source files and output executables
SRCS = bell-offload.cpp 
OBJS = $(SRCS:.cpp=.o) 
EXES = $(SRCS:.cpp=)  

# Header file
HDR = omp_q.h

# Default target
all: $(EXES)

# Compile the source files into object files
%.o: %.cpp $(HDR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Link object files into executables
$(EXES): %: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< 

# Clean up the compiled files
clean:
	rm -f $(OBJS) $(EXES)
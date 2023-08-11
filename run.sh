PYTHON_LIB=/work/z04/shared/quantum-gpu/miniconda3/envs/qiskit/lib
NLOPT_LIB=/work/z04/z04/jlee7-qu/llvm-quantum-omp/OpenMP-Quantum/nlopt/install/lib64
export LD_LIBRARY_PATH=$PYTHON_LIB:$NLOPT_LIB:$LD_LIBRARY_PATH

SIMULATOR_DIR=/work/z04/z04/jlee7-qu/llvm-quantum-omp/OpenMP-Quantum
export PYTHONPATH=$PYTHONPATH:$SIMULATOR_DIR

LLVM_INSTALL_DIR=/work/z04/z04/jlee7-qu/llvm-quantum-omp/llvm-install
CXX_LIB=/work/z04/z04/jlee7-qu/llvm-quantum-omp/llvm-install/lib/x86_64-unknown-linux-gnu
export LD_LIBRARY_PATH=$LLVM_INSTALL_DIR/lib:$CXX_LIB:$LD_LIBRARY_PATH
export PATH=$LLVM_INSTALL_DIR/bin:$PATH





#OMP_TARGET_OFFLOAD=MANDATORY OMP_DEFAULT_DEVICE=1 LIBOMPTARGET_INFO=-1 ./bell-offload
OMP_TARGET_OFFLOAD=MANDATORY OMP_DEFAULT_DEVICE=1 LIBOMPTARGET_INFO=-1 ./vqe-offload
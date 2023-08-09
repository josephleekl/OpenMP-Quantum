PYTHON_LIB=/work/z04/shared/quantum-gpu/miniconda3/envs/qiskit/lib
export LD_LIBRARY_PATH=$PYTHON_LIB:$LD_LIBRARY_PATH

SIMULATOR_DIR=/work/z04/z04/jlee7-qu/llvm-quantum-omp/OpenMP-Quantum
export PYTHONPATH=$PYTHONPATH:$SIMULATOR_DIR

LLVM_INSTALL_DIR=/work/z04/z04/jlee7-qu/llvm-quantum-omp/llvm-install
export LD_LIBRARY_PATH=$LLVM_INSTALL_DIR/lib:$LD_LIBRARY_PATH
export PATH=$LLVM_INSTALL_DIR/bin:$PATH





OMP_TARGET_OFFLOAD=MANDATORY OMP_DEFAULT_DEVICE=1 LIBOMPTARGET_INFO=-1 ./bell-offload
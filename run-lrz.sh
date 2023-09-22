# Build library and examples

LLVMLIB=/home/debian/joseph/llvm/llvm-install/lib
LLVMCXXLIB=/home/debian/joseph/llvm/llvm-install/lib/x86_64-unknown-linux-gnu




SIMULATOR_DIR=/home/debian/joseph/OpenMP-Quantum/src
export PYTHONPATH=$PYTHONPATH:$SIMULATOR_DIR

export LD_LIBRARY_PATH=$LLVMLIB:$LLVMCXXLIB:$LD_LIBRARY_PATH
# Python simulation version
OMP_TARGET_OFFLOAD=MANDATORY OMP_DEFAULT_DEVICE=1 LIBOMPTARGET_INFO=-1  ./build/examples/bell-offload
# Quantum device version
#OMP_TARGET_OFFLOAD=MANDATORY OMP_DEFAULT_DEVICE=1 LIBOMPTARGET_INFO=-1  ./build/examples/bell-offload-lrz

# Python simulation version
#OMP_TARGET_OFFLOAD=MANDATORY OMP_DEFAULT_DEVICE=1 LIBOMPTARGET_INFO=-1 ./build/examples/vqe-offload
# Python simulation version
#OMP_TARGET_OFFLOAD=MANDATORY OMP_DEFAULT_DEVICE=1 LIBOMPTARGET_INFO=-1  ./build/examples/qaoa-offload
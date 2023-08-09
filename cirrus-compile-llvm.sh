#!/usr/bin/bash


module load cmake/3.25.2
module load gcc/10.2.0
module load ninja

export SRC_DIR=/work/z04/z04/jlee7-qu/llvm-quantum-omp/llvm-project
export BUILD_DIR_BOOTSTRAP=build-bootstrap
export BUILD_DIR_FINAL=build-final
export INSTALL_DIR=/work/z04/z04/jlee7-qu/llvm-quantum-omp/llvm-install

mkdir -p $INSTALL_DIR
cd $SRC_DIR

cmake -S llvm -B ${BUILD_DIR_BOOTSTRAP} -G Ninja \
-DCMAKE_C_COMPILER=gcc \
-DCMAKE_CXX_COMPILER=g++ \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_TARGETS_TO_BUILD=Native \
-DLLVM_ENABLE_ASSERTIONS=ON \
-DLLVM_ENABLE_PROJECTS='clang;lld'







ninja -C ${BUILD_DIR_BOOTSTRAP}


export LD_LIBRARY_PATH="${SRC_DIR}/${BUILD_DIR_BOOTSTRAP}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
export PATH="${SRC_DIR}/${BUILD_DIR_BOOTSTRAP}/bin${PATH:+:${PATH}}"


cmake -S llvm -B ${BUILD_DIR_FINAL} -G Ninja \
-DCMAKE_C_COMPILER=clang \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_TARGETS_TO_BUILD=Native \
-DLLVM_ENABLE_ASSERTIONS=ON \
-DLLVM_ENABLE_PROJECTS='clang;lld' \
-DLLVM_ENABLE_RUNTIMES='openmp;libcxx;libcxxabi;libunwind' 

ninja -C ${BUILD_DIR_FINAL} install


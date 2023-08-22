# OpenMP-Quantum
A small C library to support offloading quantum circuit computation via OpenMP target interface. 
# Dependencies
- Python and Qiskit required for QASM simulation. 
- LLVM with OpenMP runtime (Currently only tested on x86_64, see `cirrus-compile-llvm.sh` for example).
- NLOPT optimizer.

# Offload
`bell-offload.cpp` gives a simple example of using OpenMP to offload a quantum region to a target device. The target device converts the quantum gates into a QASM file `circuit.qasm`, then calls the Qiskit simulator in Python. The QIR file is also generated in `circuit.ll`.

`vqe-offload.cpp` and `qaoa-offload.cpp` give examples of variational quantum eigensolver (VQE), which uses a classical optimizer to obtain optimum parameters for a quantum state which minimizes the expected energy of a given hamiltonian, as computed on a quantum target. 
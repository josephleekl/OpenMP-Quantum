# OpenMP-Quantum

# Dependencies
- Python and Qiskit required for QASM simulation. 
- LLVM with OpenMP runtime (see `cirrus-compile-llvm.sh` for example).
- NLOPT optimizer.

# Offload
`bell-offload.cpp` gives a simple example of using OpenMP to offload a quantum region to a target device. The target device converts the quantum gates into a QASM file `circuit.qasm`, then calls the Qiskit simulator in Python. `run.sh` gives the required runtime flags. 

`vqe-offload.cpp` gives an example of a variational quantum eigensolver (VQE), which uses a classical optimizer to obtain optimum parameters for a quantum state which minimizes the expected energy of a given hamiltonian, as computed on a quantum targetß. 
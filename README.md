# OpenMP-Quantum

# Dependencies
- Python and Qiskit required for QASM simulation. 
- LLVM with OpenMP runtime (see `cirrus-compile-llvm.sh` for example).

# Offload
`bell-offload.cpp` gives a simple example of using OpenMP to offload a quantum region to a target device. The target device converts the quantum gates into a QASM file `circuit.qasm`, then calls the Qiskit simulator in Python. `run.sh` gives the required runtime flags. 
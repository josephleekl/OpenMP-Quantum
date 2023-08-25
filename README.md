# OpenMP-Quantum
A small C library to support offloading quantum circuit computation via OpenMP target interface. 
# Dependencies
- Python and Qiskit required for QASM simulation. 
- LLVM with OpenMP runtime (Currently only tested on x86_64, see `cirrus-compile-llvm.sh` for example).
- NLOPT optimizer.
- JANSSON and RABBITMQ for communication with LRZ quantum daemon.


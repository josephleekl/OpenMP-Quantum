# OpenMP-Quantum
A small C library to support offloading quantum circuit computation via OpenMP target interface. 

Applications in C/C++ can use this library to offload quantum circuit computation to a separate target device (e.g. another x86 core), which manages the communication with an actual quantum device or an emulator, keeping the main thread free for classical computation while waiting for the quantum result.

# Components
Currently the main components of the library includes:
- `omp_q` : Manages circuits, operators, and measurement interface. Output circuits into OpenQASM 2.0 and QIR.
- `simulate_qiskit`: Invokes Python Qiskit to simulate QASM circuit.
- `simulate_lrz`: Communicates with LRZ quantum daemon (QLM or quantum hardware).

# Dependencies
To build this library, there are a few dependencies:
- [LLVM with OpenMP](https://openmp.llvm.org/) runtime (Currently only tested on x86_64, see `cirrus-compile-llvm.sh` or `lrz-compile-llvm.sh` for configuration example).
- Python and [Qiskit](https://qiskit.org/) required for QASM simulation. 
- [NLOPT](https://nlopt.readthedocs.io/en/latest/) optimizer for VQE/QAOA examples
- [JANSSON](https://github.com/akheron/jansson) and [RABBITMQ-C](https://github.com/alanxz/rabbitmq-c) for communication with LRZ quantum daemon.

# Scripts
- Use `lrz-compile-llvm.sh` to build a LLVM with OpenMP runtime
- Use `build-lrz.sh` to build `libOMPQ` and examples in `build/`
- Use `run-lrz.sh` to run example

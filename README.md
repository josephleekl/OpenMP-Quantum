# OpenMP-Quantum

# Dependencies
Python and Qiskit required for QASM simulation.

## Bell example output:
num threads = 10

omp_q QASM string:
"""
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
h q[0];
cx q[0], q[1];
creg c[2];
measure q -> c;

"""
omp_q QASM saved to: test.qasm

Python: Loading QASM file...
Python: Running QASM simulation...
Python: Result:
{'00': 499, '11': 525}
Measurement result: state | 00 > , Count: 499
Measurement result: state | 11 > , Count: 525

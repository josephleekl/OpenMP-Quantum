#ifndef SIMULATE_QISKIT_H
#define SIMULATE_QISKIT_H

#include <Python.h>

void omp_q_python_initialize();

void omp_q_python_finalize();

int qiskit_simulate(const char *qasm_filename, int num_qubits, double meas_probabilities[], int shots);

#endif // SIMULATE_QISKIT_H

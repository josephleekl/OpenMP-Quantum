#include <omp.h>
#include <iostream>
#include "omp_q.h"

void bell_0()
{
    // Run on target device
    // TO DO: create and map back measurement result
#pragma omp target
    {
        // qregs contain string of QASM instructions
        omp_q_reg qregs = omp_create_q_reg(2);
        omp_q_h(qregs, 0);
        omp_q_cx(qregs, 0, 1);

        // QASM saved to 'circuit.qasm', then simulate with Qiskit,
        omp_q_measure(qregs);
           
    }
}

int main()
{
    bell_0();
    return 0;
}

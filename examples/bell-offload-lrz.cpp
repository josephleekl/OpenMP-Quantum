#include <omp.h>
#include <iostream>
extern "C"
{
#include "omp_q.h"
}

#define NUMQUBITS 2
void bell_0()
{
    double probabilities[1 << NUMQUBITS];
    // Run on target device
#pragma omp target map(from:probabilities)
    {
        // qregs contain string of QASM instructions
        omp_q_reg *qregs = omp_create_q_reg(NUMQUBITS);
        omp_q_h(qregs, 0);
        omp_q_cx(qregs, 0, 1);

        // QASM saved to 'circuit.qasm'
        omp_q_measure(qregs, probabilities);
        omp_destroy_q_reg(qregs);

    }

    omp_q_print_probabilities(probabilities, NUMQUBITS);
}

int main()
{
    bell_0();
    return 0;
}

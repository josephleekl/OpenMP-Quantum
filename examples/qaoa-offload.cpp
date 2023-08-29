#include <omp.h>
#include <iostream>
#include <nlopt.h>
extern "C"
{
#include "omp_q.h"
}

#include <bitset>

#define NUMQUBIT 4
/*
This example demonstrates a max-cut QAOA implementation.
We use a 2-layer trial function with 8 parameters,
and the problem has 4 nodes/qubits which are connected
in a square. The optimimum cut solutions are
(0101) and (1010).

Reference:
https://pennylane.ai/qml/demos/tutorial_qaoa_maxcut
*/

// Prepare quantum state given 8 angle parameters: 2 layers
void prepareState(omp_q_reg *q_reg , const double* parameters)
{
    omp_q_h(q_reg, 0);
    omp_q_h(q_reg, 1);
    omp_q_h(q_reg, 2);
    omp_q_h(q_reg, 3);
    // U_C
    omp_q_cx(q_reg, 0, 1);
    omp_q_rz(q_reg, 1, parameters[0]);
    omp_q_cx(q_reg, 0, 1);
    omp_q_cx(q_reg, 0, 3);
    omp_q_rz(q_reg, 3, parameters[0]);
    omp_q_cx(q_reg, 0, 3);
    omp_q_cx(q_reg, 1, 2);
    omp_q_rz(q_reg, 2, parameters[0]);
    omp_q_cx(q_reg, 1, 2);
    omp_q_cx(q_reg, 2, 3);
    omp_q_rz(q_reg, 3, parameters[0]);
    omp_q_cx(q_reg, 2, 3);

    // U_B
    omp_q_rx(q_reg, 0, parameters[1] * 2.);
    omp_q_rx(q_reg, 1, parameters[1] * 2.);
    omp_q_rx(q_reg, 2, parameters[1] * 2.);
    omp_q_rx(q_reg, 3, parameters[1] * 2.);

    // U_C
    omp_q_cx(q_reg, 0, 1);
    omp_q_rz(q_reg, 1, parameters[2]);
    omp_q_cx(q_reg, 0, 1);
    omp_q_cx(q_reg, 0, 3);
    omp_q_rz(q_reg, 3, parameters[2]);
    omp_q_cx(q_reg, 0, 3);
    omp_q_cx(q_reg, 1, 2);
    omp_q_rz(q_reg, 2, parameters[2]);
    omp_q_cx(q_reg, 1, 2);
    omp_q_cx(q_reg, 2, 3);
    omp_q_rz(q_reg, 3, parameters[2]);
    omp_q_cx(q_reg, 2, 3);

    // U_B
    omp_q_rx(q_reg, 0, parameters[3] * 2.);
    omp_q_rx(q_reg, 1, parameters[3] * 2.);
    omp_q_rx(q_reg, 2, parameters[3] * 2.);
    omp_q_rx(q_reg, 3, parameters[3] * 2.);
}

int vqe_interation_count = 0;

double vqe_quantum_evaluation(unsigned n, const double* angles, double* grad, void* f_data)
{
    ++vqe_interation_count;


    double energy;
    double probabilities[16] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    #pragma omp target map(to: angles) map(from:probabilities, energy)
    {
    
        energy = 0.;
        omp_q_reg *q_regs = omp_create_q_reg(NUMQUBIT);
        prepareState(q_regs, angles);

        // Graph: [(0, 1), (0, 3), (1, 2), (2, 3)]
        omp_q_observable *hamiltonian = omp_q_create_observable(4);
        omp_q_observable_add_term(hamiltonian, "IIZZ", 1.);
        omp_q_observable_add_term(hamiltonian, "ZZII", 1.);
        omp_q_observable_add_term(hamiltonian, "ZIIZ", 1.);
        omp_q_observable_add_term(hamiltonian, "IZZI", 1.);

        energy = omp_q_expval(q_regs, hamiltonian, probabilities);
        omp_destroy_q_reg(q_regs);
        omp_destroy_q_observable(hamiltonian);
    
    }
    
    

    std::cout << "Iteration " << vqe_interation_count << "  | energy = " << energy << std::endl;
    std::cout << "Parameters: \n";
    for (int i = 0; i < 4; i++)
    {
        std::cout << "p["<<i<<"] = " << angles[i] << std::endl;
    }

    std::cout << "Probability distribution: \n";
    omp_q_print_probabilities(probabilities, NUMQUBIT);
    return energy;

    
}






int main(){
    //omp_q_python_initialize();

    nlopt_opt classical_optimizer;
    classical_optimizer = nlopt_create(NLOPT_GN_CRS2_LM, 4);
    double lower_bounds[4] = {-2., -2., -2., -2.};
    double upper_bounds[4] = {2., 2., 2., 2.};
    nlopt_set_lower_bounds(classical_optimizer, lower_bounds);
    nlopt_set_upper_bounds(classical_optimizer, upper_bounds);

    // Initial guess:
    double angles[4] = {.1, -.1, .1, .1};
    nlopt_set_min_objective(classical_optimizer, vqe_quantum_evaluation, NULL);
    nlopt_set_ftol_rel(classical_optimizer, 1e-2);
    vqe_quantum_evaluation(0,angles,NULL,NULL); // Evaluate once
    
    double minEnergy;
    //nlopt_optimize(classical_optimizer, angles, &minEnergy); // Minimizer
    //std::cout << "found minimum after " << vqe_interation_count << " evaluations. Minimum energy = " << minEnergy << std::endl;

    nlopt_destroy(classical_optimizer);
    //omp_q_python_finalize();
    return 0;
}
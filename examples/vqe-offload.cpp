#include <omp.h>
#include <iostream>
#include <nlopt.h>

extern "C" {
    #include "omp_q.h"
}
/*
This example demonstrates a VQE implementation, looking
for the lowest Eigenvalue of a Hamiltonian.

The reference minimum value is -1.85727

Reference:
https://qiskit.org/ecosystem/algorithms/tutorials/01_algorithms_introduction.html
 */

#define NUMQUBITS 2
int vqe_interation_count = 0;
// 

// Prepare quantum state given angle parameters
void prepareState(omp_q_reg *q_reg , const double* parameters)
{
    omp_q_ry(q_reg, 0, parameters[0]);
    omp_q_ry(q_reg, 1, parameters[1]);
    omp_q_cz(q_reg, 1, 0);
    omp_q_ry(q_reg, 0, parameters[2]);
    omp_q_ry(q_reg, 1, parameters[3]);
    omp_q_cz(q_reg, 1, 0);
    omp_q_ry(q_reg, 0, parameters[4]);
    omp_q_ry(q_reg, 1, parameters[5]);
    omp_q_cz(q_reg, 1, 0);
    omp_q_ry(q_reg, 0, parameters[6]);
    omp_q_ry(q_reg, 1, parameters[7]);
}

double vqe_quantum_evaluation(unsigned n, const double* angles, double* grad, void* f_data)
{
    ++vqe_interation_count;

    // Hamiltonian : 
    /* ​H2_op = [
        ("II", -1.052373245772859),
        ("IZ", 0.39793742484318045),
        ("ZI", -0.39793742484318045),
        ("ZZ", -0.01128010425623538),
        ("XX", 0.18093119978423156)
    ] */
    double energy;
    #pragma omp target map(to: angles) map(from:energy)
    {

    double probabilities[1 << NUMQUBITS] = {0., 0., 0., 0.};
    omp_q_reg *q_regs = omp_create_q_reg(NUMQUBITS);
    prepareState(q_regs, angles);
    omp_q_observable *hamiltonian = omp_q_create_observable(NUMQUBITS);
    energy = -1.052373245772859; // II
    omp_q_observable_add_term(hamiltonian, "IZ",  0.39793742484318045);
    omp_q_observable_add_term(hamiltonian, "ZI", -0.39793742484318045);
    omp_q_observable_add_term(hamiltonian, "ZZ", -0.01128010425623538);
    omp_q_observable_add_term(hamiltonian, "XX", 0.18093119978423156);


    energy += omp_q_expval(q_regs, hamiltonian, probabilities);
    
    omp_destroy_q_observable(hamiltonian);
    omp_destroy_q_reg(q_regs);
    }
    std::cout << "Iteration " << vqe_interation_count << "  | energy = " << energy << std::endl;
    std::cout << "Parameters: \n";
    for (int i = 0; i < 8; i++)
    {
        std::cout << "p[" << i << "] = " << angles[i] << std::endl;
    }

    return energy;
}






int main(){
    omp_q_python_initialize();

    //vqe_quantum_evaluation(0,angles,NULL,NULL); // Evaluate once

    nlopt_opt classical_optimizer;
    classical_optimizer = nlopt_create(NLOPT_GN_CRS2_LM, 8);

    // Parameters bound for optimizer
    double lower_bounds[8] = {4., -6., -4., 2., -5., 1., 4., -5.};
    double upper_bounds[8] = {5., -5., -3., 3., -4., 2., 5., -4. };
    nlopt_set_lower_bounds(classical_optimizer, lower_bounds);
    nlopt_set_upper_bounds(classical_optimizer, upper_bounds);


    // Initial guess:
    // Example optimum parameters: {4.56396229, -5.75034766, -3.00830468, 2.13314842, -4.22436638, 1.78635307, 4.28554374, -4.53347953};
    double angles[8] = {4.1, -5.1, -3.1, 2.1, -4.1, 1.1, 4.1, -4.1};

    // More optimizer settings:
    nlopt_set_ftol_rel(classical_optimizer, 1e-3);
    //nlopt_set_maxeval(classical_optimizer, 3);
    
    // Optimize:
    double minEnergy;
    nlopt_set_min_objective(classical_optimizer, vqe_quantum_evaluation, NULL);
    nlopt_optimize(classical_optimizer, angles, &minEnergy); // Minimizer


    std::cout << "found minimum after " << vqe_interation_count << " evaluations. Minimum energy = " << minEnergy << std::endl;
    std::cout << "reference value = -1.85727" << std::endl;
    
    nlopt_destroy(classical_optimizer);
    omp_q_python_finalize();
    return 0;
}
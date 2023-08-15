#include <omp.h>
#include <iostream>
#include <nlopt.h>
#include "omp_q.h"
#include <bitset>


int vqe_interation_count = 0;
// https://pennylane.ai/qml/demos/tutorial_qaoa_maxcut

// Prepare quantum state given angle parameters: 2 layers
void prepareState(omp_q_reg &q_reg , const double* parameters)
{
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

double vqe_quantum_evaluation(unsigned n, const double* angles, double* grad, void* f_data)
{
    ++vqe_interation_count;

    // Graph: [(0, 1), (0, 3), (1, 2), (2, 3)]

    double energy;
    double probabilities[16] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    #pragma omp target map(to: angles) map(from:probabilities, energy)
    {

        energy = 0.;
        omp_q_reg q_regs = omp_create_q_reg(4);
        prepareState(q_regs, angles);
        omp_q_measure(q_regs, probabilities);

        // (0, 1)
        energy +=  (
            probabilities[0] - probabilities[1] - probabilities[2] + probabilities[3]
            +probabilities[4] - probabilities[5] - probabilities[6] + probabilities[7]
            +probabilities[8] - probabilities[9] - probabilities[10] + probabilities[11]
            +probabilities[12] - probabilities[13] - probabilities[14] + probabilities[15]
        ); // signs from eigenvalues of eigenstates

        // (0, 3)
        energy +=  (
            probabilities[0] - probabilities[1] + probabilities[2] - probabilities[3]
            +probabilities[4] - probabilities[5] + probabilities[6] - probabilities[7]
            -probabilities[8] + probabilities[9] - probabilities[10] + probabilities[11]
            -probabilities[12] + probabilities[13] - probabilities[14] + probabilities[15]
        ); // signs from eigenvalues of eigenstates

         // (1, 2)
        energy += (
            probabilities[0] + probabilities[1] - probabilities[2] - probabilities[3]
            +probabilities[4] + probabilities[5] - probabilities[6] - probabilities[7]
            +probabilities[8] + probabilities[9] - probabilities[10] - probabilities[11]
            +probabilities[12] + probabilities[13] - probabilities[14] - probabilities[15]
        ); // signs from eigenvalues of eigenstates

         // (0, 3)
        energy +=  (
            probabilities[0] + probabilities[1] + probabilities[2] + probabilities[3]
            -probabilities[4] - probabilities[5] - probabilities[6] - probabilities[7]
            -probabilities[8] - probabilities[9] - probabilities[10] - probabilities[11]
            +probabilities[12] + probabilities[13] + probabilities[14] + probabilities[15]
        ); // signs from eigenvalues of eigenstates
    
    }
    
    

    std::cout << "Iteration " << vqe_interation_count << "  | energy = " << energy << std::endl;
    std::cout << "Parameters: \n";
    for (int i = 0; i < 4; i++)
    {
        std::cout << "p["<<i<<"] = " << angles[i] << std::endl;
    }

    std::cout << "Probability distribution: \n";
    for (int i = 0; i < 16; i++)
    {
        std::cout << "probabilities[" << std::bitset<4>(i) << "] = " << probabilities[i] << std::endl;
    }
    return energy;

    
}






int main(){
    omp_q_python_initialize();

    nlopt_opt classical_optimizer;
    classical_optimizer = nlopt_create(NLOPT_GN_CRS2_LM, 4);
    double lower_bounds[4] = {0., 0., 0., 0.};
    double upper_bounds[4] = {3., 3., 3., 3.};
    nlopt_set_lower_bounds(classical_optimizer, lower_bounds);
    nlopt_set_upper_bounds(classical_optimizer, upper_bounds);

    // Initial guess:
    double angles[4] = {.1, .1, .1, .1};
    nlopt_set_min_objective(classical_optimizer, vqe_quantum_evaluation, NULL);
    nlopt_set_ftol_rel(classical_optimizer, 1e-3);
    //vqe_quantum_evaluation(0,angles,NULL,NULL); // Evaluate once
    
    double minEnergy;
    nlopt_optimize(classical_optimizer, angles, &minEnergy); // Minimizer
    std::cout << "found minimum after " << vqe_interation_count << " evaluations. Minimum energy = " << minEnergy << std::endl;

    nlopt_destroy(classical_optimizer);
    omp_q_python_finalize();
    return 0;
}
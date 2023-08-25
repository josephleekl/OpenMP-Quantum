#include <omp.h>
#include <iostream>
#include <nlopt.h>
#include "omp_q.h"

int count = 0;
// https://qiskit.org/documentation/tutorials/algorithms/01_algorithms_introduction.html
void prepareState(omp_q_reg &q_reg , const double* parameters)
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


double vqe(unsigned n, const double* angles, double* grad, void* f_data)
{

    ++count;
    // Initialize trial parameters
    
    double probabilities[4] = {0., 0., 0., 0.};

    // Hamiltonian
    // Basis change matrix: U_Z = I, U_X = H, U_Y = HSZ
    // II

    
    omp_q_reg q_regs = omp_create_q_reg(2);
    double Energy = -1.052373245772859;

    // IZ
    prepareState(q_regs, angles);
    omp_q_measure(q_regs, probabilities);
    Energy += 0.39793742484318045 * (probabilities[0] - probabilities[1] + probabilities[2] - probabilities[3]);

    // ZI
    omp_q_reset(q_regs);
    prepareState(q_regs, angles);
    omp_q_measure(q_regs, probabilities);
    Energy += -0.39793742484318045 * (probabilities[0] + probabilities[1] - probabilities[2] - probabilities[3]);

    // ZZ
    omp_q_reset(q_regs);
    prepareState(q_regs, angles);
    omp_q_measure(q_regs, probabilities);
    Energy += -0.01128010425623538 * (probabilities[0] - probabilities[1] - probabilities[2] + probabilities[3]);

    // XX
    omp_q_reset(q_regs);
    prepareState(q_regs, angles);
    omp_q_h(q_regs, 0);
    omp_q_h(q_regs, 1);
    omp_q_measure(q_regs, probabilities);
    Energy += 0.18093119978423156 * (probabilities[0] - probabilities[1] - probabilities[2] + probabilities[3]);

    std::cout << "Iteration " << count << "  | Energy = " << Energy << std::endl;
    std::cout << "Parameters: \n";
    for (int i = 0; i < 8; i++)
    {
        std::cout << "p["<<i<<"] = " << angles[i] << std::endl;
    }
    return Energy;

    /* ​H2_op = SparsePauliOp.from_list([
        ("II", -1.052373245772859),
        ("IZ", 0.39793742484318045),
        ("ZI", -0.39793742484318045),
        ("ZZ", -0.01128010425623538),
        ("XX", 0.18093119978423156)
    ]) */
}


int main(){
    omp_q_python_initialize();

    nlopt_opt opt;
    opt = nlopt_create(NLOPT_GN_CRS2_LM, 8);
    double lb[8] = {
        4.,
        -6.,
        -4.,
        2.,
        -5.,
        1.,
        -4.,
        -5.,
    };
    double ub[8] = {
        5.,
        -5.,
        -3.,
        3.,
        -4.,
        2.,
        5.,
        -4.,
    };
    nlopt_set_lower_bounds(opt, lb);
    nlopt_set_upper_bounds(opt, ub);

    //double angles[8] = {4.56396229, -5.75034766, -3.00830468, 2.13314842, -4.22436638,
    double angles[8] = {4.1, -5.1, -3.1, 2.1, -4.1, 1.1, 4.1, -4.1};
    //                    1.78635307, 4.28554374, -4.53347953};
    double minEnergy;
    nlopt_set_min_objective(opt, vqe, NULL);

    nlopt_set_ftol_rel(opt, 1e-3);
    vqe(0,angles,NULL,NULL);
    //nlopt_optimize(opt, angles, &minEnergy);
    std::cout << "found minimum after " << count << " evaluations. Minimum energy = " << minEnergy << std::endl;
    nlopt_destroy(opt);
    omp_q_python_finalize();
    return 0;
}
#include <omp.h>
#include <iostream>
#include "omp_q.h"

void bell_0()
{

// #pragma omp target
#pragma omp parallel
    {
#pragma omp single nowait
        {
            std::cout << "num threads = " << omp_get_num_threads() << std::endl;
#pragma omp task
            {
                omp_q_reg qregs = omp_create_q_reg(2);
                omp_q_h(qregs, 0);
                omp_q_cx(qregs, 0, 1);

                measurement result;
                omp_q_measure(qregs, result);
                for (const auto &pair : result)
                {
                    std::cout << "Measurement result: state | " << pair.first << " > , Count: " << pair.second << std::endl;
                }
            }
        }
    }
}

int main()
{
    bell_0();
    return 0;
}

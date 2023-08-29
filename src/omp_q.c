
#include <stdio.h>
#include <string.h>
#include <math.h>
// For Qiskit Simulator

#include "omp_q.h"
//   call Python Qiskit simulator
// quantum registers and operations in QASM

void omp_q_reset(struct omp_q_reg *q_reg)
{
    memset(q_reg->qasm_ops, '\0', sizeof(q_reg->qasm_ops));
    sprintf(q_reg->qasm_ops, "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[%u];\n", q_reg->num_q);
    memset(q_reg->qir_main, '\0', sizeof(q_reg->qir_main));
    memset(q_reg->qir_define, '\0', sizeof(q_reg->qir_define));
    memset(q_reg->qir_declare, '\0', sizeof(q_reg->qir_declare));
    for (int i = 0; i < MAX_OBSERVABLE_TERMS; i++) q_reg->qir_declared[i] = 0;
}
struct omp_q_reg* omp_create_q_reg(int num_q)
{
    struct omp_q_reg *q_reg = (struct omp_q_reg *)malloc(sizeof(struct omp_q_reg));
    ;
    if (q_reg != NULL)
    {
        q_reg->num_q = num_q;
        omp_q_reset(q_reg);
        return q_reg;
    }
    else
    {
        return NULL; // Memory allocation failed
    }

}

void omp_destroy_q_reg(struct omp_q_reg* q_reg)
{
    if (q_reg != NULL)
    {
        free(q_reg);
    }
    else
    {
        printf("q_reg free error\n");
    }
}

// Quantum Gates (specified in qelib1.inc)

void omp_q_cx(struct omp_q_reg *q_reg, int control, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "cx q[%u], q[%u];\n", control, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_cx] = 1;
    free(buffer);
    
}

void omp_q_u3(struct omp_q_reg *q_reg, int target, double theta, double phi, double lambda) 
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "u3(%f,%f,%f) q[%u];\n", theta, phi, lambda, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @custom__quantum__u3__body(double %f, double %f,double %f, %%%%qubit* %%%%q%u)\n", theta, phi, lambda, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_u3] = 1;
    free(buffer);
}

void omp_q_u2(struct omp_q_reg *q_reg, int target, double phi, double lambda) 
{

    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "u2(%f,%f) q[%u];\n", phi, lambda, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @custom__quantum__u3__body(double %f, double %f,double %f, %%%%qubit* %%%%q%u)\n", M_PI / 2., phi, lambda, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_u3] = 1;
    free(buffer);
}

void omp_q_u1(struct omp_q_reg *q_reg, int target, double lambda) 
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "u1(%f) q[%u];\n", lambda, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @custom__quantum__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n",lambda, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_r] = 1;
    free(buffer);
}

void omp_q_id(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "id q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    free(buffer);
}

void omp_q_x(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "x q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__x__body(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_x] = 1;
    free(buffer);
    
}

void omp_q_y(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "y q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__y__body(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_y] = 1;
    free(buffer);
}

void omp_q_h(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "h q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);
    q_reg->qir_declared[gates_h] = 1;
    free(buffer);
}

void omp_q_z(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "z q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__z__body(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_z] = 1;
    free(buffer);
}

void omp_q_s(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "s q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__s__body(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_s] = 1;
    free(buffer);
}

void omp_q_sdg(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "sdg q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__s__adj(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_sdg] = 1;
    free(buffer);
}

void omp_q_t(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "t q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__t__body(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_t] = 1;
    free(buffer);
}

void omp_q_tdg(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "tdg q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__t__adj(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_tdg] = 1;
    free(buffer);
}


void omp_q_rx(struct omp_q_reg *q_reg, int target, double theta)
{

    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "rx(%f) q[%u];\n", theta, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 1, double %f, %%%%qubit* %%%%q%u)\n", theta, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_r] = 1;
    free(buffer);

}

void omp_q_ry(struct omp_q_reg *q_reg, int target, double theta)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "ry(%f) q[%u];\n", theta, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 3, double %f, %%%%qubit* %%%%q%u)\n", theta, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_r] = 1;
    free(buffer);
}

void omp_q_rz(struct omp_q_reg *q_reg, int target, double phi)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "rz(%f) q[%u];\n", phi, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", phi, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_r] = 1;
    free(buffer);
}

void omp_q_sx(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "sx q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__s__adj(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__s__adj(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_h] = 1;
    q_reg->qir_declared[gates_sdg] = 1;
    free(buffer);
}

void omp_q_sxdg(struct omp_q_reg *q_reg, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "sxdg q[%u];\n", target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__s__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__s__body(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_h] = 1;
    q_reg->qir_declared[gates_s] = 1;
    free(buffer);
}

void omp_q_cz(struct omp_q_reg *q_reg, int control, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "cz q[%u], q[%u];\n", control, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "  call void @__quantum__qis__cz__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_cz] = 1;
    free(buffer);
}

void omp_q_cy(struct omp_q_reg *q_reg, int control, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "cy q[%u], q[%u];\n", control, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "  call void @__quantum__qis__s__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__s__adj(%%%%qubit* %%%%q%u)\n", target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_cx] = 1;
    q_reg->qir_declared[gates_s] = 1;
    q_reg->qir_declared[gates_sdg] = 1;
    free(buffer);
}

void omp_q_ch(struct omp_q_reg *q_reg, int control, int target)
{


    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "ch q[%u], q[%u];\n", control, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__s__adj(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__t__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__t__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__s__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__x__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__s__adj(%%%%qubit* %%%%q%u)\n", control);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_s] = 1;
    q_reg->qir_declared[gates_sdg] = 1;
    q_reg->qir_declared[gates_cx] = 1;
    q_reg->qir_declared[gates_h] = 1;
    q_reg->qir_declared[gates_x] = 1;
    q_reg->qir_declared[gates_t] = 1;
    free(buffer);
}

void omp_q_swap(struct omp_q_reg *q_reg, int target_1, int target_2)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "swap q[%u], q[%u];\n", target_1, target_2);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", target_1, target_2);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", target_2, target_1);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", target_1, target_2);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_cx] = 1;
    free(buffer);
}

void omp_q_ccx(struct omp_q_reg *q_reg, int control_1, int control_2, int target)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "ccx q[%u], q[%u], q[%u];\n", control_1, control_2, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control_2, target);

    sprintf(buffer, "  call void @__quantum__qis__t__adj(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control_1, target);

    sprintf(buffer, "  call void @__quantum__qis__t__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control_2, target);

    sprintf(buffer, "  call void @__quantum__qis__t__adj(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control_1, target);

    sprintf(buffer, "  call void @__quantum__qis__t__body(%%%%qubit* %%%%q%u)\n", control_2);
    sprintf(buffer, "  call void @__quantum__qis__t__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__h__body(%%%%qubit* %%%%q%u)\n", target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control_1, control_2);

    sprintf(buffer, "  call void @__quantum__qis__t__body(%%%%qubit* %%%%q%u)\n", control_1);
    sprintf(buffer, "  call void @__quantum__qis__t__adj(%%%%qubit* %%%%q%u)\n", control_2);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control_1, control_2);

    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_cx] = 1;
    q_reg->qir_declared[gates_h] = 1;
    q_reg->qir_declared[gates_t] = 1;
    q_reg->qir_declared[gates_tdg] = 1;
    free(buffer);

}

void omp_q_crz(struct omp_q_reg *q_reg, int control, int target, double lambda)
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "crz(%f) q[%u], q[%u];\n", lambda, control, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", lambda/2., target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", (-1.) * lambda / 2., target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_cx] = 1;
    q_reg->qir_declared[gates_r] = 1;
    free(buffer);
}

void omp_q_cu1(struct omp_q_reg *q_reg, int control, int target, double lambda)
{

    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "cu1(%f) q[%u], q[%u];\n", lambda, control, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", lambda / 2., control);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", (-1.) * lambda / 2., target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", lambda / 2., target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_cx] = 1;
    q_reg->qir_declared[gates_r] = 1;
    free(buffer);
}

void omp_q_cu3(struct omp_q_reg *q_reg, int control, int target, double theta, double phi, double lambda) 
{
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "cu3(%f, %f, %f) q[%u], q[%u];\n", theta, phi, lambda, control, target);
    strcat(q_reg->qasm_ops, buffer);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", lambda + phi, control);
    sprintf(buffer, "  call void @__quantum__qis__r__body(i2 2, double %f, %%%%qubit* %%%%q%u)\n", lambda - phi, target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__u3__body(double %f, double 0.0, double %f, %%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", theta / 2., (-1.) * (phi + lambda) / 2., target);
    sprintf(buffer, "  call void @__quantum__qis__cx__body(%%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", control, target);
    sprintf(buffer, "  call void @__quantum__qis__u3__body(double %f, double %f, double 0.0, %%%%qubit* %%%%q%u, %%%%qubit* %%%%q%u)\n", theta / 2., phi, target);
    strcat(q_reg->qir_main, buffer);

    q_reg->qir_declared[gates_u3] = 1;
    q_reg->qir_declared[gates_cx] = 1;
    q_reg->qir_declared[gates_r] = 1;
    free(buffer);
}

void QIRaddDefine(struct omp_q_reg *q_reg) 
{
    if (q_reg->qir_declared[gates_u3])
    {
        strcat(q_reg->qir_define, "define internal void @custom__quantum__u3__body(double theta, double phi, double lambda,%%%%qubit* %target) {\n");
        strcat(q_reg->qir_define, "entry:\n");
        strcat(q_reg->qir_define, "  call void @__quantum__qis__r__body(i2 2, double lambda, % Qubit * % q )\n");
        strcat(q_reg->qir_define, "  call void @__quantum__qis__r__body(i2 3, double theta, % Qubit * % q )\n");
        strcat(q_reg->qir_define, "  call void @__quantum__qis__r__body(i2 2, double phi, % Qubit * % q )\n");
        strcat(q_reg->qir_define, "  ret void\n");
        strcat(q_reg->qir_define, "}\n");
        
        q_reg->qir_declared[gates_r] = 1;
    }

}
void QIRaddDeclare(struct omp_q_reg *q_reg)
{
    if (q_reg->qir_declared[gates_cx])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__cx__body(%%qubit*, %%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_x])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__x__body(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_y])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__y__body(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_h])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__h__body(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_z])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__z__body(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_s])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__s__body(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_sdg])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__s__adj(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_t])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__t__body(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_tdg])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__t__adj(%%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_r])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__r__body(i2, double, %%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_cz])
    {
        strcat(q_reg->qir_declare, "declare void @__quantum__qis__cz__body(%%qubit*, %%qubit*)\n\n");
    }
    if (q_reg->qir_declared[gates_u3])
    {
        strcat(q_reg->qir_declare, "declare void @custom__quantum__u3__body(double, double, double, %%qubit*)\n\n");
    }
}




// Simulate and measure
int omp_q_measure(struct omp_q_reg *q_reg, double meas_probabilities[])
{
    // Save to qasm string to file
    char qasmfilename[] = "circuit.qasm";
    FILE *qasmfile = fopen(qasmfilename, "w");
    char *buffer = (char *)malloc(1000);
    sprintf(buffer, "creg c[%u];\nmeasure q -> c;\n", q_reg->num_q);
    strcat(q_reg->qasm_ops, buffer);
    free(buffer);
    fprintf(qasmfile, q_reg->qasm_ops);
    fclose(qasmfile);

    char qirfilename[] = "circuit.ll";
    FILE *qirfile = fopen(qirfilename, "w");

    fprintf(qirfile, "; type definitions\n\n");
    fprintf(qirfile, "%%Qubit = type opaque\n");
    fprintf(qirfile, "%%Result = type opaque\n\n");

    fprintf(qirfile, "; global constants (labels for output recording)\n\n");
    for (int count = 0; count < q_reg->num_q; ++count)
        fprintf(qirfile, "@%u internal constant [3 x i8] c\"r%u\\00\"\n", count, count);
    fprintf(qirfile, "\n");

    fprintf(qirfile, "; entry point definition\n\n");
    fprintf(qirfile, "define void @main() #0{\n");
    fprintf(qirfile, "entry:\n");
    fprintf(qirfile, "  call void @__quantum__rt__initialize(i8 * null) \n");
    fprintf(qirfile, q_reg->qir_main);

    fprintf(qirfile, "  call void @__quantum__qis__mz__body(%%qubit* null, %Result* null)\n");
    for (int count = 1; count < q_reg->num_q; ++count)
        fprintf(qirfile, "  call void @__quantum__qis__mz__body(%%qubit* inttoptr (i64 %u to %%qubit*), %Result* writeonly inttoptr (i64 %u to %Result*))\n", count, count);
    fprintf(qirfile, "  call void @__quantum__rt__tuple_record_output(i64 %u, i8 * null)\n", q_reg->num_q);
    fprintf(qirfile, "  call void @__quantum__rt__result_record_output(%Result* null, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @0, i32 0, i32 0))\n");
    for (int count = 1; count < q_reg->num_q; ++count)
        fprintf(qirfile, "  call void @__quantum__rt__result_record_output(%Result* inttoptr(i64 %u to %Result*), i8 * getelementptr inbounds([3 x i8], [3 x i8] * @%u, i32 0, i32 0))\n", count, count);
    fprintf(qirfile, "}\n\n");

    QIRaddDefine(q_reg);
    QIRaddDeclare(q_reg);
    fprintf(qirfile, q_reg->qir_declare);
    fprintf(qirfile, "declare void @__quantum__rt__initialize(i8*)\n\n");
    fprintf(qirfile, "declare void @__quantum__rt__tuple_record_output(i64, i8 *)\n\n");
    fprintf(qirfile, "declare void @__quantum__rt__result_record_output(%Result*, i8 *)\n\n");
    fprintf(qirfile, q_reg->qir_define);

    fprintf(qirfile, "; attributes\n\n");
    fprintf(qirfile, "attributes #0 = { \" entry_point \" \" qir_profiles \"=\" base_profile \" \" output_labeling_schema \"=\" schema_id \" \" required_num_qubits \"=\"%u\" \" required_num_results \"=\"\" }\n\n", q_reg->num_q, q_reg->num_q);
    fprintf(qirfile, "attributes #1 = { \"irreversible\" }\n\n");

    fprintf(qirfile, "; module flags\n\n");
    fprintf(qirfile, "!llvm.module.flags = !{!0, !1, !2, !3}\n\n");
    fprintf(qirfile, "!0 = ! { i32 1, !\"qir_major_version\", i32 1 }\n");
    fprintf(qirfile, "!1 = ! { i32 7, !\"qir_minor_version\", i32 0 }\n");
    fprintf(qirfile, "!2 = ! { i32 1, !\"dynamic_qubit_management\", i1 false }\n");
    fprintf(qirfile, "!3 = ! { i32 1, !\"dynamic_result_management\", i1 false }\n");
    fclose(qirfile);

    // Simulate
    //qiskit_simulate(qasmfilename, q_reg->num_q, meas_probabilities, SHOTS);
    lrz_measure(q_reg, SHOTS, meas_probabilities);
    
    
    omp_q_reset(q_reg);

    return 0;
}




struct omp_q_observable* omp_q_create_observable(int num_q)
{
    struct omp_q_observable *q_observable = (struct omp_q_observable *)malloc(sizeof(struct omp_q_observable));
    if (q_observable != NULL)
    {
        q_observable->num_q = num_q;
        return q_observable;
    }
    else
    {
        return NULL; // Memory allocation failed
    }
}

void omp_destroy_q_observable(struct omp_q_observable* q_observable)
{
    if (q_observable != NULL)
    {
        free(q_observable);
    }
    else
    {
        printf("q_reg free error\n");
    }
}

    void omp_q_observable_add_term(struct omp_q_observable *q_observable, const char *pauli_string, const double coefficient)
{
    q_observable->pauli_strings[q_observable->num_terms] = (int *)malloc(sizeof(int) * q_observable->num_q);
    assert(strlen(pauli_string) == q_observable->num_q);
    for (int qubit = 0; qubit < q_observable->num_q; qubit++)
    {
        switch (pauli_string[q_observable->num_q - qubit - 1])
        {
        case 'I':
            q_observable->pauli_strings[q_observable->num_terms][qubit] = pauli_I;
            break;
        case 'X':
            q_observable->pauli_strings[q_observable->num_terms][qubit] = pauli_X;
            break;
        case 'Y':
            q_observable->pauli_strings[q_observable->num_terms][qubit] = pauli_Y;
            break;
        case 'Z':
            q_observable->pauli_strings[q_observable->num_terms][qubit] = pauli_Z;
            break;
        }
    }
    q_observable->coeffs[q_observable->num_terms] = coefficient;
    q_observable->num_terms += 1;
}

double omp_q_expval(const struct omp_q_reg *q_reg, struct omp_q_observable *observable, double* probabilities)
{
    assert(q_reg->num_q == observable->num_q);
    struct omp_q_reg *q_reg_copy = omp_create_q_reg(q_reg->num_q);



    double value = 0;
    int eigenvalues[q_reg->num_q];
    for (int term = 0; term < observable->num_terms; term++)
    {
    strcpy(q_reg_copy->qasm_ops, q_reg->qasm_ops);
    strcpy(q_reg_copy->qir_main, q_reg->qir_main);
    strcpy(q_reg_copy->qir_define, q_reg->qir_define); 
    strcpy(q_reg_copy->qir_declare, q_reg->qir_declare);
    for(int i = 0; i < MAX_OBSERVABLE_TERMS; i++)
        q_reg_copy->qir_declared[i] = q_reg->qir_declared[i];
    double single_term_value = 0;
        for (int qubit = 0; qubit < q_reg->num_q; qubit++)
        {
            switch (observable->pauli_strings[term][qubit])
            {
            case pauli_I:
                eigenvalues[qubit] = 1;
                break;
            case pauli_X:
                omp_q_h(q_reg_copy, qubit);
                eigenvalues[qubit] = 0;
                break;
            case pauli_Y:
                omp_q_z(q_reg_copy, qubit);
                omp_q_s(q_reg_copy, qubit);
                omp_q_h(q_reg_copy, qubit);
                eigenvalues[qubit] = 0;
                break;
            case pauli_Z:
                eigenvalues[qubit] = 0;
                break;
            }
        }

        omp_q_measure(q_reg_copy, probabilities);
   
        for (int state = 0; state < (1 << q_reg_copy->num_q); state++)
        {
            
            int sign = 1;
            for (int qubit = 0; qubit < q_reg_copy->num_q; qubit++)
            {
                int bit = (state >> qubit) & 1;
                sign = (sign == (!bit || eigenvalues[qubit]));

            }
            single_term_value += (sign ? 1. : -1.) * probabilities[state];

        }
        value += observable->coeffs[term] * single_term_value;
        free(observable->pauli_strings[term]);
    }
    omp_destroy_q_reg(q_reg_copy);
    return value;
}

void omp_q_print_probabilities(double probabilities[], int num_q)
{  
    int bitstring[num_q];
    for(int state = 0; state < (1<<num_q); state++)
    {
        printf("State: |");
        int state_copy = state;
        for(int i = 0; i < num_q; i++)
        {
            bitstring[i] = state_copy % 2;
            state_copy /= 2;
        }
        for(int i = num_q - 1; i >= 0; i--)
        {
            printf("%u", bitstring[i]);
        }

        printf(">  : probabiltity = %f\n", probabilities[state]);
    }

}
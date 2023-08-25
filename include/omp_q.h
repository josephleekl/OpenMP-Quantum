#ifndef OMP_Q_H
#define OMP_Q_H

#include <stdio.h>
#include <string.h>
#include <math.h>
// For Qiskit Simulator
#include <Python.h>

#include <omp_q.h>

#define MAX_STRING_SIZE 16384
#define MAX_OP_SIZE 65536
#define MAX_OBSERVABLE_TERMS 20
#define SHOTS 1000
struct omp_q_reg
{
    int num_q;
    char qasm_ops[MAX_STRING_SIZE];
    char qir_declare[MAX_STRING_SIZE];
    char qir_main[MAX_STRING_SIZE];
    char qir_define[MAX_STRING_SIZE];
    int qir_declared[MAX_OBSERVABLE_TERMS];
};

enum gates
{
    gates_cx,
    gates_x,
    gates_y,
    gates_h,
    gates_z,
    gates_s,
    gates_sdg,
    gates_t,
    gates_tdg,
    gates_r,
    gates_cz,
    gates_u3,
    gates_gatenum
};

enum pauli
{
    pauli_I,
    pauli_X,
    pauli_Y,
    pauli_Z
};

void omp_q_python_initialize();

void omp_q_python_finalize();

int qiskit_simulate(const char *qasm_filename, int num_qubits, double meas_probabilities[], int shots);

void omp_q_reset(struct omp_q_reg *q_reg);

struct omp_q_reg *omp_create_q_reg(int num_q);
void omp_destroy_q_reg(struct omp_q_reg *q_reg);

void omp_q_cx(struct omp_q_reg *q_reg, int control, int target);

void omp_q_u3(struct omp_q_reg *q_reg, int target, double theta, double phi, double lambda);

void omp_q_u2(struct omp_q_reg *q_reg, int target, double phi, double lambda);

void omp_q_u1(struct omp_q_reg *q_reg, int target, double lambda);

void omp_q_id(struct omp_q_reg *q_reg, int target);

void omp_q_x(struct omp_q_reg *q_reg, int target);

void omp_q_y(struct omp_q_reg *q_reg, int target);

void omp_q_h(struct omp_q_reg *q_reg, int target);

void omp_q_z(struct omp_q_reg *q_reg, int target);

void omp_q_s(struct omp_q_reg *q_reg, int target);

void omp_q_sdg(struct omp_q_reg *q_reg, int target);

void omp_q_t(struct omp_q_reg *q_reg, int target);

void omp_q_tdg(struct omp_q_reg *q_reg, int target);

void omp_q_rx(struct omp_q_reg *q_reg, int target, double theta);

void omp_q_ry(struct omp_q_reg *q_reg, int target, double theta);

void omp_q_rz(struct omp_q_reg *q_reg, int target, double phi);
void omp_q_sx(struct omp_q_reg *q_reg, int target);
void omp_q_sxdg(struct omp_q_reg *q_reg, int target);
void omp_q_cz(struct omp_q_reg *q_reg, int control, int target);
void omp_q_cy(struct omp_q_reg *q_reg, int control, int target);
void omp_q_ch(struct omp_q_reg *q_reg, int control, int target);
void omp_q_swap(struct omp_q_reg *q_reg, int target_1, int target_2);
void omp_q_ccx(struct omp_q_reg *q_reg, int control_1, int control_2, int target);
void omp_q_crz(struct omp_q_reg *q_reg, int control, int target, double lambda);
void omp_q_cu1(struct omp_q_reg *q_reg, int control, int target, double lambda);
void omp_q_cu3(struct omp_q_reg *q_reg, int control, int target, double theta, double phi, double lambda);
void QIRaddDefine(struct omp_q_reg *q_reg);
void QIRaddDeclare(struct omp_q_reg *q_reg);

int omp_q_measure(struct omp_q_reg *q_reg, double meas_probabilities[]);

struct omp_q_observable
{
    int num_q;
    int num_terms;
    int *pauli_strings[MAX_OBSERVABLE_TERMS];
    double coeffs[MAX_OBSERVABLE_TERMS];
};

struct omp_q_observable *omp_q_create_observable(int num_q);
void omp_destroy_q_observable(struct omp_q_observable *q_observable);

void omp_q_observable_add_term(struct omp_q_observable *q_observable, const char *pauli_string, const double coefficient);

double omp_q_expval(const struct omp_q_reg *q_reg, struct omp_q_observable *observable, double *probabilities);

void omp_q_print_probabilities(double probabilities[], int num_q);


#endif // OMP_Q_H

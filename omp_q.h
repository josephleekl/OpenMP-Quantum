#ifndef omp_q_H
#define omp_q_H

#include <string>
#include <iostream>
#include <fstream>

// For Qiskit Simulator
#include <Python.h>

// Call Python Qiskit simulator
int qiskit_simulate(std::string qasm_filename) 111
{
    Py_Initialize();
    PyObject *pName, *pModule, *pFunc, *pArgs, *pReturnValue;
    pName = PyUnicode_DecodeFSDefault("qasm_simulator");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    PyObject *pQasmFile = PyUnicode_DecodeUTF8(qasm_filename.c_str(), strlen(qasm_filename.c_str()), "replace");
    if (pModule != NULL)
    {
        pFunc = PyObject_GetAttrString(pModule, "simulate");
        if (pFunc && PyCallable_Check(pFunc))
        {
            pArgs = PyTuple_Pack(1, pQasmFile);
            pReturnValue = PyObject_CallObject(pFunc, pArgs);
            if (pReturnValue != NULL && PyDict_Check(pReturnValue))
            {
                // TO UPDATE: change measurement result data type
                /* PyObject *pKey, *pValue;
                Py_ssize_t pos = 0;
                while (PyDict_Next(pReturnValue, &pos, &pKey, &pValue))
                {
                    std::string key = PyUnicode_AsUTF8(pKey);
                    int value = PyLong_AsLong(pValue);
                    result[key] = value;
                } */
                Py_DECREF(pReturnValue);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return 1;
            }
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", "simulate");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", qasm_filename.c_str());
        return 1;
    }
    Py_Finalize();
    return 0;
}

// quantum registers and operations in QASM
struct omp_q_reg
{
    int num_q = 0;
    std::string ops_qasm = "";
};

omp_q_reg omp_create_q_reg(int num_q)
{
    omp_q_reg q_reg;
    q_reg.num_q = num_q;
    q_reg.ops_qasm = "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[" + std::to_string(num_q) + "];\n";
    return q_reg;
}

// Quantum Gates (specified in qelib1.inc)

void omp_q_cx(omp_q_reg &q_reg, int control, int target)
{
    q_reg.ops_qasm += "cx q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
}

void omp_q_u3(omp_q_reg &q_reg, int target, double theta, double phi, double lambda)
{
    q_reg.ops_qasm += "u3(" + std::to_string(theta) + "," + std::to_string(phi) + "," + std::to_string(lambda) + ") q[" + std::to_string(target) + "];\n";
}

void omp_q_u2(omp_q_reg &q_reg, int target, double phi, double lambda)
{
    q_reg.ops_qasm += "u2(" + std::to_string(phi) + "," + std::to_string(lambda) + ") q[" + std::to_string(target) + "];\n";
}

void omp_q_u1(omp_q_reg &q_reg, int target, double lambda)
{
    q_reg.ops_qasm += "u1(" + std::to_string(lambda) + ") q[" + std::to_string(target) + "];\n";
}

void omp_q_id(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "id q[" + std::to_string(target) + "];\n";
}

void omp_q_x(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "x q[" + std::to_string(target) + "];\n";
}
void omp_q_y(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "y q[" + std::to_string(target) + "];\n";
}
void omp_q_h(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "h q[" + std::to_string(target) + "];\n";
}
void omp_q_z(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "z q[" + std::to_string(target) + "];\n";
}
void omp_q_s(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "s q[" + std::to_string(target) + "];\n";
}
void omp_q_sdg(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "sdg q[" + std::to_string(target) + "];\n";
}
void omp_q_t(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "t q[" + std::to_string(target) + "];\n";
}
void omp_q_tdg(omp_q_reg &q_reg, int target)
{
    q_reg.ops_qasm += "tdg q[" + std::to_string(target) + "];\n";
}
void omp_q_rx(omp_q_reg &q_reg, double theta)
{
    q_reg.ops_qasm += "rx(" + std::to_string(theta) + ") q[" + std::to_string(target) + "];\n";
}
void omp_q_ry(omp_q_reg &q_reg, double theta)
{
    q_reg.ops_qasm += "ry(" + std::to_string(theta) + ") q[" + std::to_string(target) + "];\n";
}
void omp_q_rz(omp_q_reg &q_reg, double phi)
{
    q_reg.ops_qasm += "rz(" + std::to_string(phi) + ") q[" + std::to_string(target) + "];\n";
}
void omp_q_cz(omp_q_reg &q_reg, int control, int target)
{
    q_reg.ops_qasm += "cz q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
}
void omp_q_cy(omp_q_reg &q_reg, int control, int target)
{
    q_reg.ops_qasm += "cy q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
}
void omp_q_ch(omp_q_reg &q_reg, int control, int target)
{
    q_reg.ops_qasm += "ch q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
}
void omp_q_ccx(omp_q_reg &q_reg, int control_1, int control_2, int target)
{
    q_reg.ops_qasm += "ccx q[" + std::to_string(control_1) + "], q[" + std::to_string(control_2) + "], q[" + std::to_string(target) + "];\n";
}
void omp_q_crz(omp_q_reg &q_reg, int control, int target, double lambda)
{
    q_reg.ops_qasm += "crz(" + std::to_string(lambda) + " q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
}
void omp_q_cu1(omp_q_reg &q_reg, int control, int target, double lambda)
{
    q_reg.ops_qasm += "cu1(" + std::to_string(lambda) + " q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
}
void omp_q_cu3(, int control, int target, double theta, double phi, double lambda) *
{
    q_reg.ops_qasm += "cu3(" + std::to_string(theta) + ", " + std::to_string(phi) + ", " + std::to_string(lambda) + " q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
}

// Simulate and measure
int omp_q_measure(omp_q_reg &q_reg)
{
    // Add final measurement QASM instruction
    q_reg.ops_qasm += "creg c[" + std::to_string(q_reg.num_q) + "];\nmeasure q -> c;\n";

    // Save to qasm string to file
    std::string qasmfilename = "circuit.qasm";
    std::ofstream qasmfile(qasmfilename);
    if (qasmfile.is_open())
    {
        qasmfile << q_reg.ops_qasm;
        qasmfile.close();
    }

    // Simulate
    qiskit_simulate(qasmfilename);
    return 0;
}

// To do:
// - declare target?
// - Measurement Result definition
// - QIR:
// call void @__quantum__qis__h(%Qubit* %qb1)
// call void @__quantum__qis__cnot(%Qubit* %qb1, %Qubit* %qb2)
//

#endif /*omp_q_H*/
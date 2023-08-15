#ifndef omp_q_H
#define omp_q_H

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <math.h>
// For Qiskit Simulator
#include <Python.h>

//   call Python Qiskit simulator
void omp_q_python_initialize()
{
    Py_Initialize();
}

void omp_q_python_finalize()
{
    Py_Finalize();
}

int qiskit_simulate(std::string qasm_filename, int num_qubits, double meas_probabilities[])
{
    
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
            pArgs = PyTuple_Pack(2, pQasmFile, PyLong_FromLong(num_qubits));
            pReturnValue = PyObject_CallObject(pFunc, pArgs);
            if (pReturnValue != NULL && PyList_Check(pReturnValue))
            {

                // Convert Python list to array of doubles
                Py_ssize_t listSize = PyList_Size(pReturnValue);

                for (Py_ssize_t i = 0; i < listSize; ++i)
                {
                    PyObject *pItem = PyList_GetItem(pReturnValue, i);
                    meas_probabilities[i] = PyFloat_AsDouble(pItem);
                }
                Py_DECREF(pReturnValue);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "call failed\n");
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
    return 0;
}

// quantum registers and operations in QASM
struct omp_q_reg
{
    int num_q = 0;
    std::string qasm_ops = "";
    std::string qir_main = "";
    std::string qir_define = "";
    std::string qir_declare = "";
    std::unordered_set<std::string> qir_declared;
};

omp_q_reg omp_create_q_reg(int num_q)
{
    omp_q_reg q_reg;
    q_reg.num_q = num_q;
    q_reg.qasm_ops = "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[" + std::to_string(num_q) + "];\n";
    return q_reg;
}

void omp_q_reset(omp_q_reg &q_reg)
{
    q_reg.qasm_ops = "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[" + std::to_string(q_reg.num_q) + "];\n";
    q_reg.qir_main = "";
    q_reg.qir_define = "";
    q_reg.qir_declare = "";
    q_reg.qir_declared = {};
}

// Quantum Gates (specified in qelib1.inc)

void omp_q_cx(omp_q_reg &q_reg, int control, int target)
{
    q_reg.qasm_ops += "cx q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("cx");    
    
}

void omp_q_u3(omp_q_reg &q_reg, int target, double theta, double phi, double lambda) 
{
    q_reg.qasm_ops += "u3(" + std::to_string(theta) + "," + std::to_string(phi) + "," + std::to_string(lambda) + ") q[" + std::to_string(target) + "];\n";

    q_reg.qir_main += "  call void @custom__quantum__u3__body(double " + std::to_string(theta) + ", double " + std::to_string(phi) + ",double " + std::to_string(lambda) + ", %Qubit* %q" + std::to_string(target) + ")\n";

    q_reg.qir_declared.insert("u3");
}

void omp_q_u2(omp_q_reg &q_reg, int target, double phi, double lambda) 
{
    q_reg.qasm_ops += "u2(" + std::to_string(phi) + "," + std::to_string(lambda) + ") q[" + std::to_string(target) + "];\n";

    q_reg.qir_main += "  call void @custom__quantum__u3__body(double " + std::to_string(M_PI / 2.) + ", double " + std::to_string(phi) + ",double " + std::to_string(lambda) + ", %Qubit* %q" + std::to_string(target) + ")\n";

    q_reg.qir_declared.insert("u3");
}

void omp_q_u1(omp_q_reg &q_reg, int target, double lambda) 
{
    q_reg.qasm_ops += "u1(" + std::to_string(lambda) + ") q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string(lambda) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("r");
}

void omp_q_id(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "id q[" + std::to_string(target) + "];\n";
}

void omp_q_x(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "x q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__x__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("x");
    
}

void omp_q_y(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "y q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__y__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("y");
    
}

void omp_q_h(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "h q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("h");
    
}

void omp_q_z(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "z q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__z__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("z");
    
}

void omp_q_s(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "s q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("s");
    
}

void omp_q_sdg(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "sdg q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("sdg");
    
}

void omp_q_t(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "t q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("t");
    
}

void omp_q_tdg(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "tdg q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("tdg");
    
}

void omp_q_rx(omp_q_reg &q_reg, int target, double theta)
{
    q_reg.qasm_ops += "rx(" + std::to_string(theta) + ") q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 1, double " + std::to_string(theta) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("r");
    
}

void omp_q_ry(omp_q_reg &q_reg, int target, double theta)
{
    q_reg.qasm_ops += "ry(" + std::to_string(theta) + ") q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 3, double " + std::to_string(theta) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("r");
}

void omp_q_rz(omp_q_reg &q_reg, int target, double phi)
{
    q_reg.qasm_ops += "rz(" + std::to_string(phi) + ") q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string(phi) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("r");
}

void omp_q_sx(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "sx q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("sdg");
    q_reg.qir_declared.insert("h");
    
}

void omp_q_sxdg(omp_q_reg &q_reg, int target)
{
    q_reg.qasm_ops += "sxdg q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("s");
    q_reg.qir_declared.insert("h");
}

void omp_q_cz(omp_q_reg &q_reg, int control, int target)
{
    q_reg.qasm_ops += "cz q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__cz__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("cz");
    
}

void omp_q_cy(omp_q_reg &q_reg, int control, int target)
{
    q_reg.qasm_ops += "cy q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_declared.insert("s");
    q_reg.qir_declared.insert("sdg");
    q_reg.qir_declared.insert("cx");

}

void omp_q_ch(omp_q_reg &q_reg, int control, int target)
{
    q_reg.qasm_ops += "ch q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__x__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__s__body(%Qubit* %q" + std::to_string(control) + ")\n";

    q_reg.qir_declared.insert("s");
    q_reg.qir_declared.insert("sdg");
    q_reg.qir_declared.insert("cx");
    q_reg.qir_declared.insert("h");
    q_reg.qir_declared.insert("x");
    q_reg.qir_declared.insert("t");
}

void omp_q_swap(omp_q_reg &q_reg, int target_1, int target_2)
{
    q_reg.qasm_ops += "swap q[" + std::to_string(target_1) + "], q[" + std::to_string(target_2) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(target_1) + ", %Qubit* %q" + std::to_string(target_2) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(target_2) + ", %Qubit* %q" + std::to_string(target_1) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(target_1) + ", %Qubit* %q" + std::to_string(target_2) + ")\n";
    q_reg.qir_declared.insert("cx");
}

void omp_q_ccx(omp_q_reg &q_reg, int control_1, int control_2, int target)
{
    q_reg.qasm_ops += "ccx q[" + std::to_string(control_1) + "], q[" + std::to_string(control_2) + "], q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control_2) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control_1) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control_2) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__adj(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control_1) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__body(%Qubit* %q" + std::to_string(control_2) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__h__body(%Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control_1) + ", %Qubit* %q" + std::to_string(control_2) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__body(%Qubit* %q" + std::to_string(control_1) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__t__adj(%Qubit* %q" + std::to_string(control_2) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control_1) + ", %Qubit* %q" + std::to_string(control_2) + ")\n";

    q_reg.qir_declared.insert("h");
    q_reg.qir_declared.insert("cx");
    q_reg.qir_declared.insert("tdg");
    q_reg.qir_declared.insert("t");
}

void omp_q_crz(omp_q_reg &q_reg, int control, int target, double lambda)
{
    q_reg.qasm_ops += "crz(" + std::to_string(lambda) + " q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string(lambda / 2.) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string((-1.) * lambda / 2.) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";

    q_reg.qir_declared.insert("r");
    q_reg.qir_declared.insert("cx");
}

void omp_q_cu1(omp_q_reg &q_reg, int control, int target, double lambda)
{
    q_reg.qasm_ops += "cu1(" + std::to_string(lambda) + " q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";

    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string(lambda / 2.) + ", %Qubit* %q" + std::to_string(control) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string((-1.) * lambda / 2.) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string(lambda / 2.) + ", %Qubit* %q" + std::to_string(target) + ")\n";

    q_reg.qir_declared.insert("r");
    q_reg.qir_declared.insert("cx");
}

void omp_q_cu3(omp_q_reg &q_reg, int control, int target, double theta, double phi, double lambda) // TO ADD
{
    q_reg.qasm_ops += "cu3(" + std::to_string(theta) + ", " + std::to_string(phi) + ", " + std::to_string(lambda) + " q[" + std::to_string(control) + "], q[" + std::to_string(target) + "];\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string(lambda + phi) + ", %Qubit* %q" + std::to_string(control) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__r__body(i2 2, double " + std::to_string(lambda - phi) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @custom__quantum__u3__body(double " + std::to_string((-1.) * theta / 2.) + ", double 0.0, double " + std::to_string(-1. * (phi + lambda) / 2.) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @__quantum__qis__cx__body(%Qubit* %q" + std::to_string(control) + ", %Qubit* %q" + std::to_string(target) + ")\n";
    q_reg.qir_main += "  call void @custom__quantum__u3__body(double " + std::to_string(theta / 2.) + ", double " + std::to_string(phi) + ", double 0.0, %Qubit* %q" + std::to_string(target) + ")\n";

    q_reg.qir_declared.insert("u3");
    q_reg.qir_declared.insert("r");
    q_reg.qir_declared.insert("cx");
}

void QIRaddDefine(omp_q_reg &q_reg) 
{
    if (q_reg.qir_declared.find("u3") != q_reg.qir_declared.end())
    {
        q_reg.qir_define += "define internal void @custom__quantum__u3__body(double theta, double phi, double lambda,%Qubit* %target) {\n";
        q_reg.qir_define += "entry:\n";
        q_reg.qir_define += "  call void @__quantum__qis__r__body(i2 2, double lambda, % Qubit * % q )\n";
        q_reg.qir_define += "  call void @__quantum__qis__r__body(i2 3, double theta, % Qubit * % q )\n";
        q_reg.qir_define += "  call void @__quantum__qis__r__body(i2 2, double phi, % Qubit * % q )\n";
        q_reg.qir_define += "  ret void\n";
        q_reg.qir_define += "}\n";

        q_reg.qir_declared.insert("r");
    }

}

void QIRaddDeclare(omp_q_reg &q_reg)
{
    if (q_reg.qir_declared.find("cx") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__cx__body(%Qubit*, %Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("x") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__x__body(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("y") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__y__body(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("h") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__h__body(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("z") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__z__body(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("s") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__s__body(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("sdg") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__s__adj(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("t") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__t__body(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("tdg") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__t__adj(%Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("r") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__r__body(i2, double, %Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("cz") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @__quantum__qis__cz__body(%Qubit*, %Qubit*)\n\n";
    }
    if (q_reg.qir_declared.find("u3") != q_reg.qir_declared.end())
    {
        q_reg.qir_declare += "declare void @custom__quantum__u3__body(double, double, double, %Qubit*)\n\n";
    }
}

// Simulate and measure
int omp_q_measure(omp_q_reg &q_reg, double meas_probabilities[])
{
    // Add final measurement QASM instruction
    q_reg.qasm_ops += "creg c[" + std::to_string(q_reg.num_q) + "];\nmeasure q -> c;\n";

    // Save to qasm string to file
    std::string qasmfilename = "circuit.qasm";
    std::ofstream qasmfile(qasmfilename);
    if (qasmfile.is_open())
    {
        qasmfile << q_reg.qasm_ops;
        qasmfile.close();
    }

    std::string qirfilename = "circuit.ll";
    std::ofstream qirfile(qirfilename);
    if (qirfile.is_open())
    {
        qirfile << "; type definitions\n\n";
        qirfile << "%Qubit = type opaque\n";
        qirfile << "%Result = type opaque\n\n";

        qirfile << "; global constants (labels for output recording)\n\n";
        for (int count = 0; count < q_reg.num_q; ++count)
            qirfile << "@" << count << " internal constant [3 x i8] c\"r" << count << "\\00\"\n";
        qirfile << "\n";

        qirfile << "; entry point definition\n\n";
        qirfile << "define void @main() #0{\n";
        qirfile << "entry:\n";
        qirfile << "  call void @__quantum__rt__initialize(i8 * null) \n";
        qirfile << q_reg.qir_main;

        
        qirfile << "  call void @__quantum__qis__mz__body(%Qubit* null, %Result* null)\n";
        for(int count = 1; count < q_reg.num_q; ++count)
            qirfile << "  call void @__quantum__qis__mz__body(%Qubit* inttoptr (i64 " << count << " to %Qubit*), %Result* writeonly inttoptr (i64 " << count << " to %Result*))\n";
        qirfile << "  call void @__quantum__rt__tuple_record_output(i64 " << q_reg.num_q << ", i8 * null)\n";
        qirfile << "  call void @__quantum__rt__result_record_output(%Result* null, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @0, i32 0, i32 0))\n";
        for (int count = 1; count < q_reg.num_q; ++count)
            qirfile << "  call void @__quantum__rt__result_record_output(%Result* inttoptr(i64 " << count << " to %Result*), i8 * getelementptr inbounds([3 x i8], [3 x i8] * @" << count << ", i32 0, i32 0))\n";
        qirfile << "}\n\n";

        QIRaddDefine(q_reg);
        QIRaddDeclare(q_reg);
        qirfile << q_reg.qir_declare;
        qirfile << "declare void @__quantum__rt__initialize(i8*)\n\n";
        qirfile << "declare void @__quantum__rt__tuple_record_output(i64, i8 *)\n\n";
        qirfile << "declare void @__quantum__rt__result_record_output(%Result*, i8 *)\n\n";
        qirfile << q_reg.qir_define;

        qirfile << "; attributes\n\n";
        qirfile << "attributes #0 = { \" entry_point \" \" qir_profiles \"=\" base_profile \" \" output_labeling_schema \"=\" schema_id \" \" required_num_qubits \"=\"" << q_reg.num_q << "\" \" required_num_results \"=\"" << q_reg.num_q << "\" }\n\n";
        qirfile << "attributes #1 = { \"irreversible\" }\n\n";

        qirfile << "; module flags\n\n";
        qirfile << "!llvm.module.flags = !{!0, !1, !2, !3}\n\n";
        qirfile << "!0 = ! { i32 1, !\"qir_major_version\", i32 1 }\n";
        qirfile << "!1 = ! { i32 7, !\"qir_minor_version\", i32 0 }\n";
        qirfile << "!2 = ! { i32 1, !\"dynamic_qubit_management\", i1 false }\n";
        qirfile << "!3 = ! { i32 1, !\"dynamic_result_management\", i1 false }\n";
        qirfile.close();
    }

    // Simulate
    qiskit_simulate(qasmfilename, q_reg.num_q, meas_probabilities);
    return 0;
}


#endif /*omp_q_H*/
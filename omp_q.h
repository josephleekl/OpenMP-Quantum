#ifndef omp_q_H
#define omp_q_H

#include <string>
#include <format>
#include <iostream>
#include <fstream>

//For Qiskit Simulator
#include <Python.h>

// Measurement result: <state, count>
#include <map>
typedef std::map<std::string, int> measurement;

// Call Python Qiskit simulator
int qiskit_simulate(std::string qasm_filename, measurement &result)
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
                // Convert Python dictionary to std::map

                PyObject *pKey, *pValue;
                Py_ssize_t pos = 0;

                while (PyDict_Next(pReturnValue, &pos, &pKey, &pValue))
                {
                    std::string key = PyUnicode_AsUTF8(pKey);
                    int value = PyLong_AsLong(pValue);
                    result[key] = value;
                }
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
struct omp_q_reg {
    int num_q = 0;
    std::string ops_qasm = ""; 
}; 

omp_q_reg omp_create_q_reg(int num_q){
    omp_q_reg q_reg;
    q_reg.num_q = num_q;
    q_reg.ops_qasm = std::vformat("OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[{}];\n", std::make_format_args(num_q));
    return q_reg;

}

// Quantum Gates
void omp_q_h(omp_q_reg &q_reg, int target){
    q_reg.ops_qasm += std::vformat("h q[{}];\n", std::make_format_args(target));
}

void omp_q_cx(omp_q_reg &q_reg, int control, int target){
    q_reg.ops_qasm += std::vformat("cx q[{}], q[{}];\n", std::make_format_args(control,  target));
}

// Simulate and measure
int omp_q_measure(omp_q_reg &q_reg, measurement &result){
    // Add final measurement QASM instruction
    q_reg.ops_qasm += std::vformat("creg c[{}];\nmeasure q -> c;\n",std::make_format_args(q_reg.num_q));
    std::cout << "\nomp_q QASM string: \n\"\"\"\n";
    std::cout << q_reg.ops_qasm;
    std::cout << "\n\"\"\"\n";

    // Save to qasm string to file
    std::string qasmfilename = "test.qasm";
    std::cout << "omp_q QASM saved to: " << qasmfilename << "\n\n\n";
    std::ofstream qasmfile(qasmfilename);
    if (qasmfile.is_open())
    {
        qasmfile << q_reg.ops_qasm;
        qasmfile.close();
    }

    // Simulate 
    qiskit_simulate(qasmfilename, result);
    return 0;
}

// To do:
// Add other gates
// QIR:
// call void @__quantum__qis__h(%Qubit* %qb1)
// call void @__quantum__qis__cnot(%Qubit* %qb1, %Qubit* %qb2)

#endif /*omp_q_H*/
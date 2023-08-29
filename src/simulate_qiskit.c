#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Python.h>

#include "omp_q.h"
#include "simulate_qiskit.h"


void omp_q_python_initialize()
{
    Py_Initialize();
}

void omp_q_python_finalize()
{
    Py_Finalize();
}

int qiskit_simulate(const char *qasm_filename, int num_qubits, double meas_probabilities[], int shots)
{
    PyObject *pName, *pModule, *pFunc, *pArgs, *pReturnValue;
    pName = PyUnicode_DecodeFSDefault("qasm_simulator"); 
     pModule = PyImport_Import(pName);
     Py_DECREF(pName);
     PyObject *pQasmFile = PyUnicode_DecodeUTF8(qasm_filename, strlen(qasm_filename), "replace");
     if (pModule != NULL)
     {
         pFunc = PyObject_GetAttrString(pModule, "simulate");

         if (pFunc && PyCallable_Check(pFunc))
         {
             pArgs = PyTuple_Pack(3, pQasmFile, PyLong_FromLong(num_qubits), PyLong_FromLong(shots));

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
         fprintf(stderr, "Failed to load \"%s\"\n", qasm_filename);
         return 1;
     }
    return 0;
}

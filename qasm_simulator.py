import qiskit.qasm2
from qiskit import BasicAer
from qiskit import transpile


def simulate(filename, num_qubits):
    shots = 100000
    #print("Python: Loading QASM file...")
    circuit = qiskit.qasm2.load(filename)
    backend = BasicAer.get_backend('qasm_simulator')
    #print("Python: Running QASM simulation...")
    job = backend.run(transpile(circuit, backend), shots=shots)
    #print("Python: Result:")
    counts = job.result().get_counts(circuit)
    #print(counts)
    all_bitstrings = [format(i, f'0{num_qubits}b') for i in range(2 ** num_qubits)]
    ordered_counts = [counts.get(bitstring, 0) for bitstring in all_bitstrings]   
    ordered_probabilities =  [num / shots for num in ordered_counts]
    #print(ordered_probabilities)
    return ordered_probabilities
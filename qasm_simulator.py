import qiskit
#from qiskit import Aer, transpile
from qiskit import BasicAer, transpile

def simulate(filename, num_qubits):
    shots = 1000
    circuit = qiskit.qasm2.load(filename)
    backend = BasicAer.get_backend('qasm_simulator')
    job = backend.run(transpile(circuit, backend), shots=shots)
    counts = job.result().get_counts(circuit)
    all_bitstrings = [format(i, f'0{num_qubits}b') for i in range(2 ** num_qubits)]
    ordered_counts = [counts.get(bitstring, 0) for bitstring in all_bitstrings]   
    ordered_probabilities =  [num / sum(ordered_counts) for num in ordered_counts]
    return ordered_probabilities
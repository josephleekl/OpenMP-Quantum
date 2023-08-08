import qiskit.qasm2
from qiskit import BasicAer
from qiskit import transpile


def simulate(filename):
    print("Python: Loading QASM file...")
    circuit = qiskit.qasm2.load(filename)
    backend = BasicAer.get_backend('qasm_simulator')
    print("Python: Running QASM simulation...")
    job = backend.run(transpile(circuit, backend))
    print("Python: Result:")
    counts = job.result().get_counts(circuit)
    print(counts)
    return counts
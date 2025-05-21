import numpy as np
from libmm_module_sw import MMSim

def test_mm_sw():
    MATRIX_SIZE = 16
    print(f"Running MM software test (via Python) with matrix size: {MATRIX_SIZE}x{MATRIX_SIZE}")

    a = np.random.randint(0, 10, size=(MATRIX_SIZE, MATRIX_SIZE), dtype=np.int32)
    b = np.random.randint(0, 10, size=(MATRIX_SIZE, MATRIX_SIZE), dtype=np.int32)

    try:
        simulator = MMSim()
    except Exception as e:
        print(f"Error initializing MMSim: {e}")
        return

    print("Running MM software simulation...")
    try:
        result_sim = simulator.run(a, b)
    except Exception as e:
        print(f"Error during MMSim.run: {e}")
        return

    expected_result = np.matmul(a, b)

    if np.array_equal(result_sim, expected_result):
        print("Test PASSED!")
    else:
        print("Test FAILED!")
        print(f"First few elements of simulated result:\n{result_sim[:3,:3]}")
        print(f"First few elements of expected result:\n{expected_result[:3,:3]}")
        diff = np.abs(result_sim - expected_result)
        print(f"Max difference: {np.max(diff)}")
        print(f"Mean difference: {np.mean(diff)}")

if __name__ == "__main__":
    test_mm_sw()

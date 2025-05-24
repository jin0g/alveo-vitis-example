import numpy as np
from libmv_module_sw import MVSim

def test_mv_sw():
    MATRIX_SIZE = 32
    print(f"Running MV software test (via Python) with matrix size: {MATRIX_SIZE}x{MATRIX_SIZE} and vector size: {MATRIX_SIZE}")

    a = np.random.randint(0, 10, size=(MATRIX_SIZE, MATRIX_SIZE), dtype=np.int32)
    x = np.random.randint(0, 10, size=MATRIX_SIZE, dtype=np.int32)

    try:
        simulator = MVSim()
    except Exception as e:
        print(f"Error initializing MVSim: {e}")
        return

    print("Running MV software simulation...")
    try:
        result_sim = simulator.run(a, x)
    except Exception as e:
        print(f"Error during MVSim.run: {e}")
        return

    expected_result = np.matmul(a, x)

    if np.array_equal(result_sim, expected_result):
        print("Test PASSED!")
    else:
        print("Test FAILED!")
        print(f"First few elements of simulated result:\n{result_sim[:5]}")
        print(f"First few elements of expected result:\n{expected_result[:5]}")
        diff = np.abs(result_sim - expected_result)
        print(f"Max difference: {np.max(diff)}")
        print(f"Mean difference: {np.mean(diff)}")

if __name__ == "__main__":
    test_mv_sw()

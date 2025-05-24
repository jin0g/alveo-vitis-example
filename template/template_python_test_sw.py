import numpy as np
from libtemplate_module_sw import TemplateSim

def test_template_sw():
    DATA_SIZE = 256
    print(f"Running template software test (via Python) with data size: {DATA_SIZE}")

    in_data = np.random.randint(0, 100, size=DATA_SIZE, dtype=np.int32)

    try:
        simulator = TemplateSim()
    except Exception as e:
        print(f"Error initializing TemplateSim: {e}")
        return

    print("Running template software simulation...")
    try:
        result_sim = simulator.run(in_data)
    except Exception as e:
        print(f"Error during TemplateSim.run: {e}")
        return

    expected_result = in_data.copy()  # Simple copy operation

    if np.array_equal(result_sim, expected_result):
        print("Test PASSED!")
    else:
        print("Test FAILED!")
        for i in range(DATA_SIZE):
            if result_sim[i] != expected_result[i]:
                print(f"Mismatch at index {i}: SIM={result_sim[i]}, Expected={expected_result[i]}")
                if i > 5:
                    print("...")
                    break

if __name__ == "__main__":
    test_template_sw()

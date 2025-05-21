import numpy as np
from libvdot_double_dsp_module_sw import VDotDoubleDspSim

def test_vdot_double_dsp_sw():
    DATA_SIZE = 256
    print(f"Running VDOT Double DSP software test (via Python) with data size: {DATA_SIZE}")

    a = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)
    b = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)

    try:
        simulator = VDotDoubleDspSim()
    except Exception as e:
        print(f"Error initializing VDotDoubleDspSim: {e}")
        return

    print("Running VDot Double DSP software simulation...")
    try:
        result_sim = simulator.run(a, b)
    except Exception as e: 
        print(f"Error during VDotDoubleDspSim.run: {e}")
        return

    expected_result = np.dot(a.astype(np.int32), b.astype(np.int32))

    if result_sim == expected_result:
        print("Test PASSED!")
        print(f"Simulated result: {result_sim}")
        print(f"Expected result: {expected_result}")
    else:
        print("Test FAILED!")
        print(f"Simulated result: {result_sim}")
        print(f"Expected result: {expected_result}")
        print(f"Difference: {result_sim - expected_result}")

if __name__ == "__main__":
    test_vdot_double_dsp_sw()

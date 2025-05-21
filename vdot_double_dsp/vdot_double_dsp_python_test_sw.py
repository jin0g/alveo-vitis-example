import numpy as np
import time
from libvdot_double_dsp_module_sw import VDotDoubleDspSim

def test_vdot_double_dsp_sw():
    DATA_SIZE = 256
    print(f"Running VDOT Double DSP software test (via Python) with data size: {DATA_SIZE}")

    try:
        simulator = VDotDoubleDspSim()
    except Exception as e:
        print(f"Error initializing VDotDoubleDspSim: {e}")
        return False

    print("\n=== Test with random data ===")
    a = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)
    b = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)

    start_time = time.time()
    expected_result = np.dot(a.astype(np.int32), b.astype(np.int32))
    numpy_time = (time.time() - start_time) * 1000  # Convert to ms

    print("Running VDot Double DSP software simulation...")
    try:
        start_time = time.time()
        result_sim = simulator.run(a, b)
        sim_time = (time.time() - start_time) * 1000  # Convert to ms
    except Exception as e: 
        print(f"Error during VDotDoubleDspSim.run: {e}")
        return False

    random_test_passed = result_sim == expected_result
    print(f"Random data test {'PASSED' if random_test_passed else 'FAILED'}")
    print(f"Simulated result: {result_sim}")
    print(f"Expected result: {expected_result}")
    print(f"Numpy execution time: {numpy_time:.3f} ms")
    print(f"Simulation execution time: {sim_time:.3f} ms")
    
    print("\n=== Test with edge cases ===")
    
    print("\nTest case 1: All maximum values")
    a_max = np.full(DATA_SIZE, 127, dtype=np.int8)
    b_max = np.full(DATA_SIZE, 127, dtype=np.int8)
    
    expected_max = np.dot(a_max.astype(np.int32), b_max.astype(np.int32))
    result_max = simulator.run(a_max, b_max)
    
    max_test_passed = result_max == expected_max
    print(f"Max values test {'PASSED' if max_test_passed else 'FAILED'}")
    print(f"Simulated result: {result_max}")
    print(f"Expected result: {expected_max}")
    
    print("\nTest case 2: All minimum values")
    a_min = np.full(DATA_SIZE, -128, dtype=np.int8)
    b_min = np.full(DATA_SIZE, -128, dtype=np.int8)
    
    expected_min = np.dot(a_min.astype(np.int32), b_min.astype(np.int32))
    result_min = simulator.run(a_min, b_min)
    
    min_test_passed = result_min == expected_min
    print(f"Min values test {'PASSED' if min_test_passed else 'FAILED'}")
    print(f"Simulated result: {result_min}")
    print(f"Expected result: {expected_min}")
    
    print("\nTest case 3: Mixed positive and negative values")
    a_mixed = np.array([127, -128, 0, 1, -1] * (DATA_SIZE // 5 + 1), dtype=np.int8)[:DATA_SIZE]
    b_mixed = np.array([-128, 127, 1, -1, 0] * (DATA_SIZE // 5 + 1), dtype=np.int8)[:DATA_SIZE]
    
    expected_mixed = np.dot(a_mixed.astype(np.int32), b_mixed.astype(np.int32))
    result_mixed = simulator.run(a_mixed, b_mixed)
    
    mixed_test_passed = result_mixed == expected_mixed
    print(f"Mixed values test {'PASSED' if mixed_test_passed else 'FAILED'}")
    print(f"Simulated result: {result_mixed}")
    print(f"Expected result: {expected_mixed}")
    
    print("\nTest case 4: Odd-sized vector")
    odd_size = 255  # Odd number
    a_odd = np.random.randint(-128, 128, size=odd_size, dtype=np.int8)
    b_odd = np.random.randint(-128, 128, size=odd_size, dtype=np.int8)
    
    expected_odd = np.dot(a_odd.astype(np.int32), b_odd.astype(np.int32))
    result_odd = simulator.run(a_odd, b_odd)
    
    odd_test_passed = result_odd == expected_odd
    print(f"Odd-sized vector test {'PASSED' if odd_test_passed else 'FAILED'}")
    print(f"Simulated result: {result_odd}")
    print(f"Expected result: {expected_odd}")
    
    all_passed = random_test_passed and max_test_passed and min_test_passed and mixed_test_passed and odd_test_passed
    
    print("\n=== Summary ===")
    print(f"Random data test: {'PASSED' if random_test_passed else 'FAILED'}")
    print(f"Max values test: {'PASSED' if max_test_passed else 'FAILED'}")
    print(f"Min values test: {'PASSED' if min_test_passed else 'FAILED'}")
    print(f"Mixed values test: {'PASSED' if mixed_test_passed else 'FAILED'}")
    print(f"Odd-sized vector test: {'PASSED' if odd_test_passed else 'FAILED'}")
    
    if all_passed:
        print("\nALL TESTS PASSED: Results match numpy calculations!")
        return True
    else:
        print("\nTEST FAILED: Some results don't match numpy calculations!")
        return False

if __name__ == "__main__":
    success = test_vdot_double_dsp_sw()
    exit(0 if success else 1)

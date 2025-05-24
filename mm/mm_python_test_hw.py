import numpy as np
import time
from libmm_module_hw import MMRunner

MEGA = 1024 * 1024

def test_mm_hw():
    MATRIX_SIZE = 16
    print(f"Running MM hardware test (via Python) with matrix size: {MATRIX_SIZE}x{MATRIX_SIZE}")

    a = np.random.randint(0, 10, size=(MATRIX_SIZE, MATRIX_SIZE), dtype=np.int32)
    b = np.random.randint(0, 10, size=(MATRIX_SIZE, MATRIX_SIZE), dtype=np.int32)
    
    XCLBIN_FILE = "mm.xclbin"
    try:
        runner = MMRunner(XCLBIN_FILE)
    except Exception as e:
        print(f"Error initializing MMRunner with {XCLBIN_FILE}: {e}")
        print(f"Please ensure '{XCLBIN_FILE}' exists and XRT is set up correctly.")
        return

    print(f"Running MM HW kernel ({XCLBIN_FILE})...")
    num_iterations = 5
    kernel_times = []
    total_times = []

    try:
        result_hw = runner.run(a, b)
    except Exception as e:
        print(f"Error during MMRunner.run (initial): {e}")
        return

    for i in range(num_iterations):
        print(f"Iteration {i+1}/{num_iterations}")
        
        iter_start_time = time.perf_counter()
        result_hw = runner.run(a, b)
        iter_end_time = time.perf_counter()
        
        python_measured_time_ms = (iter_end_time - iter_start_time) * 1000.0
        kernel_time_ms = runner.get_kernel_execution_time_ms()
        total_time_ms = runner.get_total_execution_time_ms()
        
        print(f"Python measured time: {python_measured_time_ms:.4f} ms")
        print(f"Kernel execution time: {kernel_time_ms:.4f} ms")
        print(f"Total execution time: {total_time_ms:.4f} ms")
        
        kernel_times.append(kernel_time_ms)
        total_times.append(total_time_ms)

    expected_result = np.matmul(a, b)

    if np.array_equal(result_hw, expected_result):
        print("Test PASSED!")
    else:
        print("Test FAILED!")
        print(f"First few elements of hardware result:\n{result_hw[:3,:3]}")
        print(f"First few elements of expected result:\n{expected_result[:3,:3]}")
        diff = np.abs(result_hw - expected_result)
        print(f"Max difference: {np.max(diff)}")
        print(f"Mean difference: {np.mean(diff)}")

    avg_kernel_time_ms = np.mean(kernel_times)
    avg_total_time_ms = np.mean(total_times)

    ops_per_run = 2 * MATRIX_SIZE**3
    
    throughput_kernel_mega_ops_per_sec = (ops_per_run / (avg_kernel_time_ms / 1000.0)) / MEGA
    throughput_total_mega_ops_per_sec = (ops_per_run / (avg_total_time_ms / 1000.0)) / MEGA 

    print("\n--- Performance Summary (HW) ---")
    print(f"Average kernel execution time: {avg_kernel_time_ms:.4f} ms")
    print(f"Average total execution time: {avg_total_time_ms:.4f} ms")
    print(f"Throughput (kernel only): {throughput_kernel_mega_ops_per_sec:.2f} M Ops/sec")
    print(f"Throughput (total): {throughput_total_mega_ops_per_sec:.2f} M Ops/sec")
    
    print("\n--- Numpy Performance Comparison ---")
    numpy_times = []
    for i in range(num_iterations):
        numpy_start = time.perf_counter()
        np.matmul(a, b)
        numpy_end = time.perf_counter()
        numpy_times.append((numpy_end - numpy_start) * 1000.0)
    
    avg_numpy_time_ms = np.mean(numpy_times)
    numpy_throughput = (ops_per_run / (avg_numpy_time_ms / 1000.0)) / MEGA
    
    print(f"Average numpy execution time: {avg_numpy_time_ms:.4f} ms")
    print(f"Numpy throughput: {numpy_throughput:.2f} M Ops/sec")
    print(f"Speedup (kernel vs numpy): {avg_numpy_time_ms / avg_kernel_time_ms:.2f}x")
    print(f"Speedup (total vs numpy): {avg_numpy_time_ms / avg_total_time_ms:.2f}x")
    
    print("Python HW test completed.")

if __name__ == "__main__":
    test_mm_hw()

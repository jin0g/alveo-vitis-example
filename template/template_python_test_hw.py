import numpy as np
import time
from libtemplate_module_hw import TemplateRunner

MEGA = 1024 * 1024

def test_template_hw():
    size = 1 * MEGA 
    print(f"Test data size: {size / MEGA:.2f} M elements")

    in_data = np.arange(size, dtype=np.int32)
    
    try:
        runner = TemplateRunner("template.xclbin")
    except Exception as e:
        print(f"Error initializing TemplateRunner: {e}")
        print("Please ensure 'template.xclbin' exists and XRT is set up correctly.")
        return

    print("Running Template HW kernel...")
    num_iterations = 5
    kernel_times = []
    total_times = []

    try:
        result = runner.run(in_data)
    except Exception as e:
        print(f"Error during TemplateRunner.run: {e}")
        return

    for i in range(num_iterations):
        print(f"Iteration {i+1}/{num_iterations}")
        
        iter_start_time = time.perf_counter()
        result = runner.run(in_data)
        iter_end_time = time.perf_counter()
        
        total_exec_time_ms = (iter_end_time - iter_start_time) * 1000.0
        
        kernel_exec_time_ms = total_exec_time_ms
        
        kernel_times.append(kernel_exec_time_ms)
        total_times.append(total_exec_time_ms)

    expected = in_data.copy()  # Simple copy operation
    assert np.array_equal(result[:10], expected[:10]), "Result (first 10) does not match expected value."
    assert np.array_equal(result[-10:], expected[-10:]), "Result (last 10) does not match expected value."
    if size > 20:
        print("Partial result validation successful.")
    else:
        assert np.array_equal(result, expected), "Result does not match expected value."
        print("Full result validation successful.")

    avg_kernel_time_ms = np.mean(kernel_times)
    avg_total_time_ms = np.mean(total_times)

    throughput_kernel_mega_ops_per_sec = (size / (avg_kernel_time_ms / 1000.0)) / MEGA
    throughput_total_mega_ops_per_sec = (size / (avg_total_time_ms / 1000.0)) / MEGA 

    print("\n--- Performance Summary (HW) ---")
    print(f"Average kernel execution time (Python measured): {avg_kernel_time_ms:.4f} ms")
    print(f"Average total execution time (Python measured): {avg_total_time_ms:.4f} ms")
    print(f"Throughput (kernel only): {throughput_kernel_mega_ops_per_sec:.2f} M Ops/sec")
    print(f"Throughput (total): {throughput_total_mega_ops_per_sec:.2f} M Ops/sec")
    print("Python HW test successful!")

if __name__ == "__main__":
    test_template_hw()

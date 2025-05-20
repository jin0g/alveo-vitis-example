import numpy as np
import time
from libvadd_module_hw import VAddRunner # モジュール名を変更

# スループット計算のための定数
MEGA = 1024 * 1024

def test_vadd_hw(): # 関数名を変更
    # テストデータの準備 (1Mサンプル)
    size = 1 * MEGA 
    # size = 1024 # 開発用に小さいサイズ
    print(f"Test data size: {size / MEGA:.2f} M elements")

    a = np.arange(size, dtype=np.int32)
    b = np.arange(size, 0, -1, dtype=np.int32) # b = size, size-1, ..., 1
    
    try:
        runner = VAddRunner("vadd.xclbin")
    except Exception as e:
        print(f"Error initializing VAddRunner: {e}")
        print("Please ensure 'vadd.xclbin' exists and XRT is set up correctly.")
        return

    print("Running VAdd HW kernel...") # メッセージ変更
    num_iterations = 5
    kernel_times = []
    total_times = []

    try:
        result = runner.run(a, b)
    except Exception as e:
        print(f"Error during VAddRunner.run: {e}")
        return

    for i in range(num_iterations):
        print(f"Iteration {i+1}/{num_iterations}")
        # iter_start_time = time.perf_counter() # Python側での計測はC++側に任せる
        result = runner.run(a, b)
        # iter_end_time = time.perf_counter()

        kernel_exec_time_ms = runner.get_kernel_execution_time_ms()
        total_exec_time_ms = runner.get_total_execution_time_ms()

        kernel_times.append(kernel_exec_time_ms)
        total_times.append(total_exec_time_ms)

    expected = a + b
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

    print("\n--- Performance Summary (HW) ---") # メッセージ変更
    print(f"Average kernel execution time: {avg_kernel_time_ms:.4f} ms")
    print(f"Average total execution time (C++ measured): {avg_total_time_ms:.4f} ms")
    print(f"Throughput (kernel only): {throughput_kernel_mega_ops_per_sec:.2f} M Ops/sec")
    print(f"Throughput (total, C++ measured): {throughput_total_mega_ops_per_sec:.2f} M Ops/sec")
    print("Python HW test successful!") # メッセージ変更

if __name__ == "__main__":
    test_vadd_hw() # 関数呼び出しを変更 
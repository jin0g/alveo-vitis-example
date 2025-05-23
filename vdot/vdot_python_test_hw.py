#!/usr/bin/env python3
#
#
#
import numpy as np
import time
from libvdot_module_hw import VDotRunner # HWモジュールをインポート

MEGA = 1024 * 1024

def test_vdot_hw():
    DATA_SIZE = 1 * MEGA
    # DATA_SIZE = 256 # For quick testing
    print(f"Running VDOT hardware test (via Python) with data size: {DATA_SIZE / MEGA:.2f} M elements")

    # テストデータの準備: char (int8) の範囲でランダムな値を生成
    a = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)
    b = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)
    
    XCLBIN_FILE = "vdot.xclbin"
    try:
        runner = VDotRunner(XCLBIN_FILE)
    except Exception as e:
        print(f"Error initializing VDotRunner with {XCLBIN_FILE}: {e}")
        print(f"Please ensure '{XCLBIN_FILE}' exists and XRT is set up correctly.")
        return

    print(f"Running VDot HW kernel ({XCLBIN_FILE})...")
    num_iterations = 5
    kernel_times = []
    total_times = []
    hw_result = 0 # Initialize hw_result

    try:
        # 最初の実行 (ウォームアップ兼)
        hw_result = runner.run(a, b)
    except Exception as e:
        print(f"Error during VDotRunner.run (initial): {e}")
        return

    for i in range(num_iterations):
        print(f"Iteration {i+1}/{num_iterations}")
        hw_result = runner.run(a, b)
        kernel_times.append(runner.get_kernel_execution_time_ms())
        total_times.append(runner.get_total_execution_time_ms())

    # 期待値の計算 (Python側, int32で計算してオーバーフローを防ぐ)
    expected_result = np.dot(a.astype(np.int32), b.astype(np.int32))

    # 結果の検証
    if hw_result == expected_result:
        print("Test PASSED!")
        print(f"Hardware result: {hw_result}")
        print(f"Expected result: {expected_result}")
    else:
        print("Test FAILED!")
        print(f"Hardware result: {hw_result}")
        print(f"Expected result: {expected_result}")
        print(f"Difference: {hw_result - expected_result}")

    avg_kernel_time_ms = np.mean(kernel_times)
    avg_total_time_ms = np.mean(total_times)

    # スループット計算: 内積演算では、N回の乗算とN-1回の加算が主要な演算
    # ここでは単純に要素数をオペレーション数とみなす（1要素あたり1 MAC相当と近似）
    # より厳密には 2*N - 1 Ops
    ops_per_run = DATA_SIZE # Simplified as N operations for dot product
    # ops_per_run = 2 * DATA_SIZE -1 # More accurate ops count

    throughput_kernel_mega_ops_per_sec = (ops_per_run / (avg_kernel_time_ms / 1000.0)) / MEGA
    throughput_total_mega_ops_per_sec = (ops_per_run / (avg_total_time_ms / 1000.0)) / MEGA 

    print("\n--- Performance Summary (HW) ---")
    print(f"Average kernel execution time: {avg_kernel_time_ms:.4f} ms")
    print(f"Average total execution time (C++ measured): {avg_total_time_ms:.4f} ms")
    print(f"Throughput (kernel only): {throughput_kernel_mega_ops_per_sec:.2f} M Ops/sec")
    print(f"Throughput (total, C++ measured): {throughput_total_mega_ops_per_sec:.2f} M Ops/sec")
    print("Python HW test completed.")

if __name__ == "__main__":
    test_vdot_hw()  
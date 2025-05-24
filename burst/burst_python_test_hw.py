#!/usr/bin/env python3
import numpy as np
import time
import argparse
import pandas as pd
from pathlib import Path

try: 
    from libbursttest_module_hw import BurstTestRunner32, BurstTestRunner64
except ImportError:
    print("Warning: libbursttest_module_hw not found. Make sure to build it first.")

MEGA = 1024 * 1024

def test_burst_transfer(bit_width, burst_length, data_size=1*MEGA):
    """Run burst transfer test for specified bit width and burst length"""
    print(f"Testing {bit_width}-bit width with burst length {burst_length}")
    
    if bit_width == 32:
        xclbin_file = f"burst_{bit_width}.xclbin"
        runner = BurstTestRunner32(xclbin_file)
        input_data = np.arange(data_size, dtype=np.int32)
    elif bit_width == 64:
        xclbin_file = f"burst_{bit_width}.xclbin"
        runner = BurstTestRunner64(xclbin_file)
        input_data = np.arange(data_size, dtype=np.int64)
    else:
        print(f"Bit width {bit_width} not supported in Python test yet")
        return None
    
    num_iterations = 5
    kernel_times = []
    total_times = []
    
    for i in range(num_iterations):
        print(f"Iteration {i+1}/{num_iterations}")
        
        result = runner.run(input_data, burst_length)
        
        kernel_time = runner.get_kernel_execution_time_ms()
        total_time = runner.get_total_execution_time_ms()
        
        kernel_times.append(kernel_time)
        total_times.append(total_time)
    
    expected = input_data + 1
    if not np.array_equal(result[:10], expected[:10]):
        print("Warning: Result verification failed for first 10 elements")
    
    avg_kernel_time_ms = np.mean(kernel_times)
    avg_total_time_ms = np.mean(total_times)
    
    bytes_per_element = bit_width // 8
    throughput_kernel_mega_ops_per_sec = (data_size / (avg_kernel_time_ms / 1000.0)) / MEGA
    throughput_total_mega_ops_per_sec = (data_size / (avg_total_time_ms / 1000.0)) / MEGA
    bandwidth_kernel_mb_per_sec = (data_size * bytes_per_element / (avg_kernel_time_ms / 1000.0)) / (1024 * 1024)
    bandwidth_total_mb_per_sec = (data_size * bytes_per_element / (avg_total_time_ms / 1000.0)) / (1024 * 1024)
    
    print("\n--- Performance Summary ---")
    print(f"Average kernel execution time: {avg_kernel_time_ms:.4f} ms")
    print(f"Average total execution time: {avg_total_time_ms:.4f} ms")
    print(f"Throughput (kernel only): {throughput_kernel_mega_ops_per_sec:.2f} M Ops/sec")
    print(f"Throughput (total): {throughput_total_mega_ops_per_sec:.2f} M Ops/sec")
    print(f"Bandwidth (kernel only): {bandwidth_kernel_mb_per_sec:.2f} MB/s")
    print(f"Bandwidth (total): {bandwidth_total_mb_per_sec:.2f} MB/s")
    
    return {
        "bit_width": bit_width,
        "burst_length": burst_length,
        "avg_kernel_time_ms": avg_kernel_time_ms,
        "avg_total_time_ms": avg_total_time_ms,
        "throughput_kernel_mega_ops_per_sec": throughput_kernel_mega_ops_per_sec,
        "throughput_total_mega_ops_per_sec": throughput_total_mega_ops_per_sec,
        "bandwidth_kernel_mb_per_sec": bandwidth_kernel_mb_per_sec,
        "bandwidth_total_mb_per_sec": bandwidth_total_mb_per_sec
    }

def main():
    parser = argparse.ArgumentParser(description='Run burst transfer tests')
    parser.add_argument('--bit-widths', type=int, nargs='+', default=[32, 64, 128, 256, 512, 1024],
                        help='Bit widths to test')
    parser.add_argument('--burst-lengths', type=int, nargs='+', default=[64, 128, 256],
                        help='Burst lengths to test (max 256)')
    parser.add_argument('--data-size', type=int, default=1*MEGA,
                        help='Data size in elements')
    args = parser.parse_args()
    
    results = []
    
    for bit_width in args.bit_widths:
        for burst_length in args.burst_lengths:
            result = test_burst_transfer(bit_width, burst_length, args.data_size)
            if result:
                results.append(result)
    
    if results:
        df = pd.DataFrame(results)
        output_file = "burst_results.csv"
        df.to_csv(output_file, index=False)
        print(f"Results saved to {output_file}")
        
        md_table = "| ビット幅 | バースト長 | スループット (M Ops/sec) | 帯域幅 (MB/s) |\n"
        md_table += "|---------|-----------|------------------------|------------|\n"
        
        for _, row in df.iterrows():
            md_table += f"| {row['bit_width']:<7} | {row['burst_length']:<9} | "
            md_table += f"{row['throughput_kernel_mega_ops_per_sec']:<24.2f} | "
            md_table += f"{row['bandwidth_kernel_mb_per_sec']:<12.2f} |\n"
        
        with open("results_table.md", "w") as f:
            f.write(md_table)
        print(f"Markdown table saved to results_table.md")

if __name__ == "__main__":
    main()

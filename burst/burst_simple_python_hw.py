#!/usr/bin/env python3

import os
import sys
import time
import numpy as np
import subprocess
import argparse
from tabulate import tabulate

def run_test(bit_width, burst_length, xclbin_file):
    """Run the test with specified bit width and burst length"""
    print(f"Running test with bit width {bit_width}, burst length {burst_length}")
    
    cmd = f"./burst_simple_hw {xclbin_file} {bit_width} {burst_length}"
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"Error running test: {result.stderr}")
        return None
    
    output = result.stdout
    
    throughput = None
    bandwidth = None
    
    for line in output.split('\n'):
        if "Throughput:" in line:
            throughput = float(line.split(':')[1].strip().split()[0])
        elif "Bandwidth:" in line:
            bandwidth = float(line.split(':')[1].strip().split()[0])
    
    return {
        "bit_width": bit_width,
        "burst_length": burst_length,
        "throughput_ops": throughput,
        "bandwidth_mb_s": bandwidth
    }

def main():
    parser = argparse.ArgumentParser(description='Run burst transfer tests')
    parser.add_argument('--bit-widths', type=str, default="32,64", 
                        help='Comma-separated list of bit widths to test')
    parser.add_argument('--burst-lengths', type=str, default="256,512,1024", 
                        help='Comma-separated list of burst lengths to test')
    args = parser.parse_args()
    
    bit_widths = [int(x) for x in args.bit_widths.split(',')]
    burst_lengths = [int(x) for x in args.burst_lengths.split(',')]
    
    results = []
    
    for bit_width in bit_widths:
        xclbin_file = f"burst_{bit_width}.xclbin"
        
        if not os.path.exists(xclbin_file):
            print(f"Warning: {xclbin_file} not found, skipping {bit_width}-bit tests")
            continue
        
        for burst_length in burst_lengths:
            result = run_test(bit_width, burst_length, xclbin_file)
            if result:
                results.append(result)
    
    if results:
        headers = ["Bit Width", "Burst Length", "Throughput (M Ops/sec)", "Bandwidth (MB/s)"]
        table_data = [
            [r["bit_width"], r["burst_length"], r["throughput_ops"], r["bandwidth_mb_s"]]
            for r in results
        ]
        
        print("\n--- Test Results ---")
        print(tabulate(table_data, headers=headers, tablefmt="grid"))
        
        with open("results_table.md", "w") as f:
            f.write("## Burst Transfer Optimization Test Results\n\n")
            f.write(tabulate(table_data, headers=headers, tablefmt="pipe"))
            f.write("\n")
    else:
        print("No test results collected")

if __name__ == "__main__":
    main()

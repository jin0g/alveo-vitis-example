/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

const char* KERNEL_NAME = "maximum_bandwidth";

inline double to_gb(double bytes) {
    return bytes / (1024.0 * 1024.0 * 1024.0);
}

inline double calculate_bandwidth(size_t bytes, double milliseconds) {
    return to_gb(bytes) / (milliseconds / 1000.0);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string xclbin_file = argv[1];

    const int DATA_SIZE = 64 * 1024 * 1024; // 各入力/出力で256MB (64M個の整数)
    const size_t DATA_BYTES = DATA_SIZE * sizeof(int);
    const size_t TOTAL_BYTES = DATA_BYTES * 8; // 4入力 + 4出力

    std::cout << "Running DDR Bandwidth Test with data size per buffer: " 
              << (DATA_BYTES / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "Total data transfer: " << (TOTAL_BYTES / (1024 * 1024)) << " MB" << std::endl;

    srand(time(nullptr));

    std::cout << "Preparing input data..." << std::endl;
    std::vector<int> source_input0(DATA_SIZE);
    std::vector<int> source_input1(DATA_SIZE);
    std::vector<int> source_input2(DATA_SIZE);
    std::vector<int> source_input3(DATA_SIZE);
    std::vector<int> result_output0(DATA_SIZE);
    std::vector<int> result_output1(DATA_SIZE);
    std::vector<int> result_output2(DATA_SIZE);
    std::vector<int> result_output3(DATA_SIZE);
    
    std::vector<int> expected_output0(DATA_SIZE);
    std::vector<int> expected_output1(DATA_SIZE);
    std::vector<int> expected_output2(DATA_SIZE);
    std::vector<int> expected_output3(DATA_SIZE);

    for (int i = 0; i < DATA_SIZE; ++i) {
        source_input0[i] = rand() % 100;
        source_input1[i] = rand() % 100;
        source_input2[i] = rand() % 100;
        source_input3[i] = rand() % 100;
        
        expected_output0[i] = source_input0[i] + 1;
        expected_output1[i] = source_input1[i] + 2;
        expected_output2[i] = source_input2[i] + 3;
        expected_output3[i] = source_input3[i] + 4;
    }

    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME);

        std::cout << "Allocating buffers..." << std::endl;
        
        auto bo_input0 = xrt::bo(device, DATA_BYTES, kernel.group_id(0));
        auto bo_input1 = xrt::bo(device, DATA_BYTES, kernel.group_id(1));
        auto bo_input2 = xrt::bo(device, DATA_BYTES, kernel.group_id(2));
        auto bo_input3 = xrt::bo(device, DATA_BYTES, kernel.group_id(3));
        
        auto bo_output0 = xrt::bo(device, DATA_BYTES, kernel.group_id(4));
        auto bo_output1 = xrt::bo(device, DATA_BYTES, kernel.group_id(5));
        auto bo_output2 = xrt::bo(device, DATA_BYTES, kernel.group_id(6));
        auto bo_output3 = xrt::bo(device, DATA_BYTES, kernel.group_id(7));

        std::cout << "Writing data to device..." << std::endl;
        
        auto transfer_start_time = std::chrono::high_resolution_clock::now();
        
        bo_input0.write(source_input0.data());
        bo_input1.write(source_input1.data());
        bo_input2.write(source_input2.data());
        bo_input3.write(source_input3.data());

        bo_input0.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_input1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_input2.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_input3.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
        auto transfer_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> transfer_to_device_ms = transfer_end_time - transfer_start_time;
        
        std::cout << "Executing kernel..." << std::endl;
        
        auto kernel_start_time = std::chrono::high_resolution_clock::now();
        
        auto run = kernel(bo_input0, bo_input1, bo_input2, bo_input3,
                         bo_output0, bo_output1, bo_output2, bo_output3,
                         DATA_SIZE);
        run.wait();
        
        auto kernel_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> kernel_duration_ms = kernel_end_time - kernel_start_time;
        
        std::cout << "Reading data from device..." << std::endl;
        
        auto read_start_time = std::chrono::high_resolution_clock::now();
        
        bo_output0.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_output1.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_output2.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_output3.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        
        bo_output0.read(result_output0.data());
        bo_output1.read(result_output1.data());
        bo_output2.read(result_output2.data());
        bo_output3.read(result_output3.data());
        
        auto read_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> transfer_from_device_ms = read_end_time - read_start_time;
        
        std::chrono::duration<double, std::milli> total_duration_ms = read_end_time - transfer_start_time;
        
        double write_bandwidth_gb_per_sec = calculate_bandwidth(DATA_BYTES * 4, transfer_to_device_ms.count());
        double kernel_bandwidth_gb_per_sec = calculate_bandwidth(DATA_BYTES * 8, kernel_duration_ms.count());
        double read_bandwidth_gb_per_sec = calculate_bandwidth(DATA_BYTES * 4, transfer_from_device_ms.count());
        double total_bandwidth_gb_per_sec = calculate_bandwidth(DATA_BYTES * 8, total_duration_ms.count());
        
        std::cout << "\n--- Performance Results ---" << std::endl;
        std::cout << "Host to Device Transfer: " << transfer_to_device_ms.count() << " ms" << std::endl;
        std::cout << "Kernel Execution: " << kernel_duration_ms.count() << " ms" << std::endl;
        std::cout << "Device to Host Transfer: " << transfer_from_device_ms.count() << " ms" << std::endl;
        std::cout << "Total Time: " << total_duration_ms.count() << " ms" << std::endl;
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n--- Bandwidth Measurements ---" << std::endl;
        std::cout << "Host to Device Bandwidth: " << write_bandwidth_gb_per_sec << " GB/s" << std::endl;
        std::cout << "Kernel Bandwidth: " << kernel_bandwidth_gb_per_sec << " GB/s" << std::endl;
        std::cout << "Device to Host Bandwidth: " << read_bandwidth_gb_per_sec << " GB/s" << std::endl;
        std::cout << "Total Effective Bandwidth: " << total_bandwidth_gb_per_sec << " GB/s" << std::endl;
        
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    bool match = true;
    const int verify_count = std::min(1000, DATA_SIZE);
    
    std::cout << "\nVerifying results (first and last " << verify_count << " elements)..." << std::endl;
    
    for (int i = 0; i < verify_count; ++i) {
        if (result_output0[i] != expected_output0[i] ||
            result_output1[i] != expected_output1[i] ||
            result_output2[i] != expected_output2[i] ||
            result_output3[i] != expected_output3[i]) {
            std::cerr << "Mismatch at start index " << i << std::endl;
            match = false;
            break;
        }
    }
    
    if (match) {
        for (int i = DATA_SIZE - verify_count; i < DATA_SIZE; ++i) {
            if (result_output0[i] != expected_output0[i] ||
                result_output1[i] != expected_output1[i] ||
                result_output2[i] != expected_output2[i] ||
                result_output3[i] != expected_output3[i]) {
                std::cerr << "Mismatch at end index " << i << std::endl;
                match = false;
                break;
            }
        }
    }

    if (match) {
        std::cout << "Test PASSED! Results match expected values." << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout << "Test FAILED! Results do not match expected values." << std::endl;
        return EXIT_FAILURE;
    }
}

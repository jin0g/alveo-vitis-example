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

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

const char* KERNEL_NAME = "mv";

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string xclbin_file = argv[1];

    const int MATRIX_SIZE = 32;
    const int MATRIX_TOTAL_SIZE = MATRIX_SIZE * MATRIX_SIZE;
    
    std::cout << "Running MV hardware test with matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE 
              << " and vector size: " << MATRIX_SIZE << std::endl;

    srand(time(nullptr));

    std::vector<int> source_a(MATRIX_TOTAL_SIZE);
    std::vector<int> source_x(MATRIX_SIZE);
    std::vector<int> result_hw(MATRIX_SIZE);
    std::vector<int> result_sw(MATRIX_SIZE);

    for (int i = 0; i < MATRIX_TOTAL_SIZE; ++i) {
        source_a[i] = rand() % 10; // Small values to avoid overflow
    }
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        source_x[i] = rand() % 10;
    }

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        result_sw[i] = 0;
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            result_sw[i] += source_a[i * MATRIX_SIZE + j] * source_x[j];
        }
    }

    try {
        auto device = xrt::device(0); // Use the first available device
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME);

        std::cout << "Allocating buffers..." << std::endl;
        auto bo_a = xrt::bo(device, MATRIX_TOTAL_SIZE * sizeof(int), kernel.group_id(0)); // Input A
        auto bo_x = xrt::bo(device, MATRIX_SIZE * sizeof(int), kernel.group_id(1)); // Input X
        auto bo_y = xrt::bo(device, MATRIX_SIZE * sizeof(int), kernel.group_id(2)); // Output Y

        std::cout << "Writing data to device..." << std::endl;
        bo_a.write(source_a.data());
        bo_x.write(source_x.data());

        bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_x.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        std::cout << "Executing kernel..." << std::endl;
        auto run_start_time = std::chrono::high_resolution_clock::now();
        auto run = kernel(bo_a, bo_x, bo_y, MATRIX_SIZE);
        run.wait();
        auto run_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> run_duration_ms = run_end_time - run_start_time;
        std::cout << "Kernel execution time: " << run_duration_ms.count() << " ms" << std::endl;

        std::cout << "Reading data from device..." << std::endl;
        bo_y.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_y.read(result_hw.data());

    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    bool match = true;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        if (result_hw[i] != result_sw[i]) {
            std::cerr << "Mismatch at index " << i << ": HW=" << result_hw[i] << ", SW=" << result_sw[i] << std::endl;
            match = false;
            break;
        }
    }

    if (match) {
        std::cout << "Test PASSED!" << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout << "Test FAILED!" << std::endl;
        return EXIT_FAILURE;
    }
}

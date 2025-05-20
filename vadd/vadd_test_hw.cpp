#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <chrono>  // For timing

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

// HLS Kernel function name (as defined in vadd.cpp and compiled into xclbin)
const char* KERNEL_NAME = "vadd";

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string xclbin_file = argv[1];

    const int DATA_SIZE = 256;
    std::cout << "Running VADD hardware test with data size: " << DATA_SIZE << std::endl;

    // Initialize random seed
    srand(time(nullptr));

    std::vector<int> source_a(DATA_SIZE);
    std::vector<int> source_b(DATA_SIZE);
    std::vector<int> result_hw(DATA_SIZE);
    std::vector<int> result_sw(DATA_SIZE);

    // Populate input vectors with random data
    for (int i = 0; i < DATA_SIZE; ++i) {
        source_a[i] = rand() % 100;
        source_b[i] = rand() % 100;
        result_sw[i] = source_a[i] + source_b[i]; // Calculate software reference
    }

    try {
        // Initialize XRT
        auto device = xrt::device(0); // Use the first available device
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME);

        std::cout << "Allocating buffers..." << std::endl;
        // Allocate buffers on the device
        auto bo_a = xrt::bo(device, DATA_SIZE * sizeof(int), kernel.group_id(0)); // Input A
        auto bo_b = xrt::bo(device, DATA_SIZE * sizeof(int), kernel.group_id(1)); // Input B
        auto bo_c = xrt::bo(device, DATA_SIZE * sizeof(int), kernel.group_id(2)); // Output C

        std::cout << "Writing data to device..." << std::endl;
        // Write data to the device buffers
        bo_a.write(source_a.data());
        bo_b.write(source_b.data());

        // Synchronize buffers to ensure data is written to device
        bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        std::cout << "Executing kernel..." << std::endl;
        auto run_start_time = std::chrono::high_resolution_clock::now();
        // Execute the kernel
        auto run = kernel(bo_a, bo_b, bo_c, DATA_SIZE);
        run.wait();
        auto run_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> run_duration_ms = run_end_time - run_start_time;
        std::cout << "Kernel execution time: " << run_duration_ms.count() << " ms" << std::endl;

        std::cout << "Reading data from device..." << std::endl;
        // Synchronize buffer to ensure data is read from device
        bo_c.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        // Read data from the device buffer
        bo_c.read(result_hw.data());

    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Compare results
    bool match = true;
    for (int i = 0; i < DATA_SIZE; ++i) {
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
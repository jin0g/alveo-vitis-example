#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <chrono>  // For timing

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

const char* KERNEL_NAME = "template_kernel";

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string xclbin_file = argv[1];

    const int DATA_SIZE = 256;
    std::cout << "Running template hardware test with data size: " << DATA_SIZE << std::endl;

    srand(time(nullptr));

    std::vector<int> source_in(DATA_SIZE);
    std::vector<int> result_hw(DATA_SIZE);
    std::vector<int> result_sw(DATA_SIZE);

    for (int i = 0; i < DATA_SIZE; ++i) {
        source_in[i] = rand() % 100;
        result_sw[i] = source_in[i]; // Calculate software reference (simple copy)
    }

    try {
        auto device = xrt::device(0); // Use the first available device
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME);

        std::cout << "Allocating buffers..." << std::endl;
        auto bo_in = xrt::bo(device, DATA_SIZE * sizeof(int), kernel.group_id(0)); // Input
        auto bo_out = xrt::bo(device, DATA_SIZE * sizeof(int), kernel.group_id(1)); // Output

        std::cout << "Writing data to device..." << std::endl;
        bo_in.write(source_in.data());

        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        std::cout << "Executing kernel..." << std::endl;
        auto run_start_time = std::chrono::high_resolution_clock::now();
        auto run = kernel(bo_in, bo_out, DATA_SIZE);
        run.wait();
        auto run_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> run_duration_ms = run_end_time - run_start_time;
        std::cout << "Kernel execution time: " << run_duration_ms.count() << " ms" << std::endl;

        std::cout << "Reading data from device..." << std::endl;
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_out.read(result_hw.data());

    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

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

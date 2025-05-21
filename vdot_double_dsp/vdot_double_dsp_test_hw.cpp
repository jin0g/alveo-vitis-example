#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>   
#include <chrono>  
#include <cstdint>

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

const char* KERNEL_NAME = "vdot_double_dsp";

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string xclbin_file = argv[1];

    const int DATA_SIZE = 256;
    std::cout << "Running VDOT Double DSP hardware test with data size: " << DATA_SIZE << std::endl;

    srand(time(nullptr));

    std::vector<int8_t> source_a(DATA_SIZE);
    std::vector<int8_t> source_b(DATA_SIZE);
    int result_hw;
    int result_sw;

    for (int i = 0; i < DATA_SIZE; ++i) {
        source_a[i] = static_cast<int8_t>((rand() % 256) - 128); // int8_t range: -128 to 127
        source_b[i] = static_cast<int8_t>((rand() % 256) - 128);
    }
    
    result_sw = 0;
    for (int i = 0; i < DATA_SIZE; ++i) {
        result_sw += static_cast<int>(source_a[i]) * static_cast<int>(source_b[i]);
    }

    try {
        auto device = xrt::device(0); 
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME);

        std::cout << "Allocating buffers..." << std::endl;
        auto bo_a = xrt::bo(device, DATA_SIZE * sizeof(int8_t), kernel.group_id(0)); 
        auto bo_b = xrt::bo(device, DATA_SIZE * sizeof(int8_t), kernel.group_id(1)); 
        auto bo_result = xrt::bo(device, sizeof(int), kernel.group_id(2));

        std::cout << "Writing data to device..." << std::endl;
        bo_a.write(source_a.data());
        bo_b.write(source_b.data());

        bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        std::cout << "Executing kernel..." << std::endl;
        auto run_start_time = std::chrono::high_resolution_clock::now();
        auto run = kernel(bo_a, bo_b, bo_result, DATA_SIZE);
        run.wait();
        auto run_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> run_duration_ms = run_end_time - run_start_time;
        std::cout << "Kernel execution time: " << run_duration_ms.count() << " ms" << std::endl;

        bo_result.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_result.read(&result_hw);
        
        std::cout << "Result from hardware: " << result_hw << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    bool match = (result_hw == result_sw);

    if (match) {
        std::cout << "Test PASSED!" << std::endl;
        std::cout << "Software result: " << result_sw << std::endl;
        std::cout << "Hardware result: " << result_hw << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout << "Test FAILED!" << std::endl;
        std::cout << "Software result: " << result_sw << std::endl;
        std::cout << "Hardware result: " << result_hw << std::endl;
        std::cout << "Difference: " << (result_sw - result_hw) << std::endl;
        return EXIT_FAILURE;
    }
}

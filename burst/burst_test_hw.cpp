#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <numeric>
#include <algorithm>

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
#include "ap_int.h"

const int DATA_SIZE = 1024 * 1024; // 1M elements
const int NUM_ITERATIONS = 5;
const int MEGA = 1024 * 1024;

const char* KERNEL_NAME_32 = "burst_32";
const char* KERNEL_NAME_64 = "burst_64";
const char* KERNEL_NAME_128 = "burst_128";
const char* KERNEL_NAME_256 = "burst_256";
const char* KERNEL_NAME_512 = "burst_512";
const char* KERNEL_NAME_1024 = "burst_1024";

void runTest32(const std::string& xclbin_file, int burst_length) {
    std::cout << "Running 32-bit test with " << DATA_SIZE << " elements and burst length " << burst_length << std::endl;
    
    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME_32);
        
        std::vector<int> source(DATA_SIZE);
        std::vector<int> result(DATA_SIZE);
        
        std::iota(source.begin(), source.end(), 0);
        
        auto bo_in = xrt::bo(device, DATA_SIZE * sizeof(int), kernel.group_id(0));
        auto bo_out = xrt::bo(device, DATA_SIZE * sizeof(int), kernel.group_id(1));
        
        bo_in.write(source.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
        std::vector<double> execution_times;
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            auto run = kernel(bo_in, bo_out, DATA_SIZE, burst_length);
            run.wait();
            
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
            execution_times.push_back(duration_ms.count());
            
            std::cout << "Iteration " << (i+1) << ": " << duration_ms.count() << " ms" << std::endl;
        }
        
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_out.read(result.data());
        
        bool match = true;
        for (int i = 0; i < std::min(10, DATA_SIZE); ++i) {
            if (result[i] != source[i] + 1) {
                std::cerr << "Mismatch at index " << i << ": result=" << result[i] 
                          << ", expected=" << (source[i] + 1) << std::endl;
                match = false;
                break;
            }
        }
        
        if (match) {
            std::cout << "Result verification successful (first 10 elements)." << std::endl;
        }
        
        double avg_time = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
        double throughput_ops = (DATA_SIZE / (avg_time / 1000.0)) / MEGA;
        double throughput_bytes = (DATA_SIZE * sizeof(int) / (avg_time / 1000.0)) / (1024 * 1024);
        
        std::cout << "\n--- Performance Summary (32-bit) ---" << std::endl;
        std::cout << "Average execution time: " << avg_time << " ms" << std::endl;
        std::cout << "Throughput: " << throughput_ops << " M Ops/sec" << std::endl;
        std::cout << "Bandwidth: " << throughput_bytes << " MB/s" << std::endl;
        
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
}

void runTest64(const std::string& xclbin_file, int burst_length) {
    std::cout << "Running 64-bit test with " << DATA_SIZE << " elements and burst length " << burst_length << std::endl;
    
    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME_64);
        
        std::vector<long long> source(DATA_SIZE);
        std::vector<long long> result(DATA_SIZE);
        
        std::iota(source.begin(), source.end(), 0);
        
        auto bo_in = xrt::bo(device, DATA_SIZE * sizeof(long long), kernel.group_id(0));
        auto bo_out = xrt::bo(device, DATA_SIZE * sizeof(long long), kernel.group_id(1));
        
        bo_in.write(source.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
        std::vector<double> execution_times;
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            auto run = kernel(bo_in, bo_out, DATA_SIZE, burst_length);
            run.wait();
            
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
            execution_times.push_back(duration_ms.count());
            
            std::cout << "Iteration " << (i+1) << ": " << duration_ms.count() << " ms" << std::endl;
        }
        
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_out.read(result.data());
        
        bool match = true;
        for (int i = 0; i < std::min(10, DATA_SIZE); ++i) {
            if (result[i] != source[i] + 1) {
                std::cerr << "Mismatch at index " << i << ": result=" << result[i] 
                          << ", expected=" << (source[i] + 1) << std::endl;
                match = false;
                break;
            }
        }
        
        if (match) {
            std::cout << "Result verification successful (first 10 elements)." << std::endl;
        }
        
        double avg_time = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
        double throughput_ops = (DATA_SIZE / (avg_time / 1000.0)) / MEGA;
        double throughput_bytes = (DATA_SIZE * sizeof(long long) / (avg_time / 1000.0)) / (1024 * 1024);
        
        std::cout << "\n--- Performance Summary (64-bit) ---" << std::endl;
        std::cout << "Average execution time: " << avg_time << " ms" << std::endl;
        std::cout << "Throughput: " << throughput_ops << " M Ops/sec" << std::endl;
        std::cout << "Bandwidth: " << throughput_bytes << " MB/s" << std::endl;
        
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
}

void runTest128(const std::string& xclbin_file, int burst_length) {
    std::cout << "Running 128-bit test with " << DATA_SIZE << " elements and burst length " << burst_length << std::endl;
    
    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME_128);
        
        int elements_per_ap_int = 128 / 32; // Assuming we pack 32-bit ints into 128-bit
        int ap_int_size = (DATA_SIZE + elements_per_ap_int - 1) / elements_per_ap_int;
        
        std::vector<int> source(DATA_SIZE);
        std::vector<int> result(DATA_SIZE);
        
        std::iota(source.begin(), source.end(), 0);
        
        auto bo_in = xrt::bo(device, ap_int_size * sizeof(ap_int<128>), kernel.group_id(0));
        auto bo_out = xrt::bo(device, ap_int_size * sizeof(ap_int<128>), kernel.group_id(1));
        
        bo_in.write(source.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
        std::vector<double> execution_times;
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            auto run = kernel(bo_in, bo_out, ap_int_size, burst_length);
            run.wait();
            
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
            execution_times.push_back(duration_ms.count());
            
            std::cout << "Iteration " << (i+1) << ": " << duration_ms.count() << " ms" << std::endl;
        }
        
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_out.read(result.data());
        
        double avg_time = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
        double throughput_ops = (DATA_SIZE / (avg_time / 1000.0)) / MEGA;
        double throughput_bytes = (DATA_SIZE * sizeof(int) * 4 / (avg_time / 1000.0)) / (1024 * 1024); // *4 for 128-bit
        
        std::cout << "\n--- Performance Summary (128-bit) ---" << std::endl;
        std::cout << "Average execution time: " << avg_time << " ms" << std::endl;
        std::cout << "Throughput: " << throughput_ops << " M Ops/sec" << std::endl;
        std::cout << "Bandwidth: " << throughput_bytes << " MB/s" << std::endl;
        
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
}

void runTest256(const std::string& xclbin_file, int burst_length) {
    std::cout << "Running 256-bit test with " << DATA_SIZE << " elements and burst length " << burst_length << std::endl;
    
    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME_256);
        
        int elements_per_ap_int = 256 / 32;
        int ap_int_size = (DATA_SIZE + elements_per_ap_int - 1) / elements_per_ap_int;
        
        std::vector<int> source(DATA_SIZE);
        std::vector<int> result(DATA_SIZE);
        
        std::iota(source.begin(), source.end(), 0);
        
        auto bo_in = xrt::bo(device, ap_int_size * sizeof(ap_int<256>), kernel.group_id(0));
        auto bo_out = xrt::bo(device, ap_int_size * sizeof(ap_int<256>), kernel.group_id(1));
        
        bo_in.write(source.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
        std::vector<double> execution_times;
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();
            auto run = kernel(bo_in, bo_out, ap_int_size, burst_length);
            run.wait();
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
            execution_times.push_back(duration_ms.count());
            
            std::cout << "Iteration " << (i+1) << ": " << duration_ms.count() << " ms" << std::endl;
        }
        
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_out.read(result.data());
        
        double avg_time = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
        double throughput_ops = (DATA_SIZE / (avg_time / 1000.0)) / MEGA;
        double throughput_bytes = (DATA_SIZE * sizeof(int) * 8 / (avg_time / 1000.0)) / (1024 * 1024); // *8 for 256-bit
        
        std::cout << "\n--- Performance Summary (256-bit) ---" << std::endl;
        std::cout << "Average execution time: " << avg_time << " ms" << std::endl;
        std::cout << "Throughput: " << throughput_ops << " M Ops/sec" << std::endl;
        std::cout << "Bandwidth: " << throughput_bytes << " MB/s" << std::endl;
        
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
}

void runTest512(const std::string& xclbin_file, int burst_length) {
    std::cout << "Running 512-bit test with " << DATA_SIZE << " elements and burst length " << burst_length << std::endl;
    
    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME_512);
        
        int elements_per_ap_int = 512 / 32;
        int ap_int_size = (DATA_SIZE + elements_per_ap_int - 1) / elements_per_ap_int;
        
        std::vector<int> source(DATA_SIZE);
        std::vector<int> result(DATA_SIZE);
        
        std::iota(source.begin(), source.end(), 0);
        
        auto bo_in = xrt::bo(device, ap_int_size * sizeof(ap_int<512>), kernel.group_id(0));
        auto bo_out = xrt::bo(device, ap_int_size * sizeof(ap_int<512>), kernel.group_id(1));
        
        bo_in.write(source.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
        std::vector<double> execution_times;
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();
            auto run = kernel(bo_in, bo_out, ap_int_size, burst_length);
            run.wait();
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
            execution_times.push_back(duration_ms.count());
            
            std::cout << "Iteration " << (i+1) << ": " << duration_ms.count() << " ms" << std::endl;
        }
        
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_out.read(result.data());
        
        double avg_time = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
        double throughput_ops = (DATA_SIZE / (avg_time / 1000.0)) / MEGA;
        double throughput_bytes = (DATA_SIZE * sizeof(int) * 16 / (avg_time / 1000.0)) / (1024 * 1024); // *16 for 512-bit
        
        std::cout << "\n--- Performance Summary (512-bit) ---" << std::endl;
        std::cout << "Average execution time: " << avg_time << " ms" << std::endl;
        std::cout << "Throughput: " << throughput_ops << " M Ops/sec" << std::endl;
        std::cout << "Bandwidth: " << throughput_bytes << " MB/s" << std::endl;
        
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
}

void runTest1024(const std::string& xclbin_file, int burst_length) {
    std::cout << "Running 1024-bit test with " << DATA_SIZE << " elements and burst length " << burst_length << std::endl;
    
    try {
        auto device = xrt::device(0);
        auto uuid = device.load_xclbin(xclbin_file);
        auto kernel = xrt::kernel(device, uuid, KERNEL_NAME_1024);
        
        int elements_per_ap_int = 1024 / 32;
        int ap_int_size = (DATA_SIZE + elements_per_ap_int - 1) / elements_per_ap_int;
        
        std::vector<int> source(DATA_SIZE);
        std::vector<int> result(DATA_SIZE);
        
        std::iota(source.begin(), source.end(), 0);
        
        auto bo_in = xrt::bo(device, ap_int_size * sizeof(ap_int<1024>), kernel.group_id(0));
        auto bo_out = xrt::bo(device, ap_int_size * sizeof(ap_int<1024>), kernel.group_id(1));
        
        bo_in.write(source.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
        std::vector<double> execution_times;
        
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();
            auto run = kernel(bo_in, bo_out, ap_int_size, burst_length);
            run.wait();
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
            execution_times.push_back(duration_ms.count());
            
            std::cout << "Iteration " << (i+1) << ": " << duration_ms.count() << " ms" << std::endl;
        }
        
        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        bo_out.read(result.data());
        
        double avg_time = std::accumulate(execution_times.begin(), execution_times.end(), 0.0) / execution_times.size();
        double throughput_ops = (DATA_SIZE / (avg_time / 1000.0)) / MEGA;
        double throughput_bytes = (DATA_SIZE * sizeof(int) * 32 / (avg_time / 1000.0)) / (1024 * 1024); // *32 for 1024-bit
        
        std::cout << "\n--- Performance Summary (1024-bit) ---" << std::endl;
        std::cout << "Average execution time: " << avg_time << " ms" << std::endl;
        std::cout << "Throughput: " << throughput_ops << " M Ops/sec" << std::endl;
        std::cout << "Bandwidth: " << throughput_bytes << " MB/s" << std::endl;
        
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file> <bit_width> [burst_length]" << std::endl;
        std::cout << "  bit_width: 32, 64, 128, 256, 512, or 1024" << std::endl;
        std::cout << "  burst_length: Optional, default is 256" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::string xclbin_file = argv[1];
    int bit_width = std::stoi(argv[2]);
    int burst_length = (argc > 3) ? std::stoi(argv[3]) : 256;
    
    std::cout << "Testing with bit width: " << bit_width << ", burst length: " << burst_length << std::endl;
    
    switch (bit_width) {
        case 32:
            runTest32(xclbin_file, burst_length);
            break;
        case 64:
            runTest64(xclbin_file, burst_length);
            break;
        case 128:
            runTest128(xclbin_file, burst_length);
            break;
        case 256:
            runTest256(xclbin_file, burst_length);
            break;
        case 512:
            runTest512(xclbin_file, burst_length);
            break;
        case 1024:
            runTest1024(xclbin_file, burst_length);
            break;
        default:
            std::cerr << "Unsupported bit width: " << bit_width << std::endl;
            return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

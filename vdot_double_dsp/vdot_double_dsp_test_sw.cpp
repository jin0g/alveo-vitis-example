#include <iostream>
#include <vector>
#include <numeric>
#include <cstdint>
#include <random>
#include <chrono>

extern "C" {
void vdot_double_dsp(const int8_t* a, const int8_t* b, int* result, int size);
}

int main() {
    const int DATA_SIZE = 256;
    std::vector<int8_t> a(DATA_SIZE);
    std::vector<int8_t> b(DATA_SIZE);
    int result_sw;
    int result_hw;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-128, 127); // int8_t range

    std::cout << "Generating random test data..." << std::endl;
    for (int i = 0; i < DATA_SIZE; ++i) {
        a[i] = static_cast<int8_t>(dist(gen));
        b[i] = static_cast<int8_t>(dist(gen));
    }

    std::cout << "Calculating software reference result..." << std::endl;
    result_sw = 0;
    for (int i = 0; i < DATA_SIZE; ++i) {
        result_sw += static_cast<int>(a[i]) * static_cast<int>(b[i]);
    }

    std::cout << "Running vdot_double_dsp implementation..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    vdot_double_dsp(a.data(), b.data(), &result_hw, DATA_SIZE);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;

    bool match = (result_sw == result_hw);
    if (match) {
        std::cout << "TEST PASSED." << std::endl;
        std::cout << "Software result: " << result_sw << std::endl;
        std::cout << "Hardware result: " << result_hw << std::endl;
        std::cout << "Execution time: " << duration_ms.count() << " ms" << std::endl;
    } else {
        std::cout << "TEST FAILED." << std::endl;
        std::cout << "Software result: " << result_sw << std::endl;
        std::cout << "Hardware result: " << result_hw << std::endl;
        std::cout << "Difference: " << (result_sw - result_hw) << std::endl;
    }

    return match ? 0 : 1;
}

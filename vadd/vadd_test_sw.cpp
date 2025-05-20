#include <iostream>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

// HLS Kernel function declaration (from vadd.cpp)
extern "C" void vadd(const int* a, const int* b, int* c, const int size);

bool run_test(int data_size) {
    std::vector<int> a(data_size);
    std::vector<int> b(data_size);
    std::vector<int> c_hw(data_size); // Result from hardware (simulated)
    std::vector<int> c_sw(data_size); // Result from software

    // Initialize random seed
    srand(time(nullptr));

    // Populate input vectors with random data
    for (int i = 0; i < data_size; ++i) {
        a[i] = rand() % 100; // Random numbers between 0 and 99
        b[i] = rand() % 100;
    }

    // Call the HLS kernel (simulated)
    vadd(a.data(), b.data(), c_hw.data(), data_size);

    // Calculate expected results (software model)
    for (int i = 0; i < data_size; ++i) {
        c_sw[i] = a[i] + b[i];
    }

    // Compare results
    for (int i = 0; i < data_size; ++i) {
        if (c_hw[i] != c_sw[i]) {
            std::cerr << "Mismatch at index " << i << ": HW=" << c_hw[i] << ", SW=" << c_sw[i] << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    const int DATA_SIZE = 256;
    std::cout << "Running VADD software test with data size: " << DATA_SIZE << std::endl;

    if (run_test(DATA_SIZE)) {
        std::cout << "Test PASSED!" << std::endl;
        return 0; // Success
    } else {
        std::cout << "Test FAILED!" << std::endl;
        return 1; // Failure
    }
} 
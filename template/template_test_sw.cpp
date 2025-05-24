#include <iostream>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

extern "C" void template_kernel(const int* in, int* out, const int size);

bool run_test(int data_size) {
    std::vector<int> in(data_size);
    std::vector<int> out_hw(data_size); // Result from hardware (simulated)
    std::vector<int> out_sw(data_size); // Result from software

    srand(time(nullptr));

    for (int i = 0; i < data_size; ++i) {
        in[i] = rand() % 100; // Random numbers between 0 and 99
    }

    template_kernel(in.data(), out_hw.data(), data_size);

    for (int i = 0; i < data_size; ++i) {
        out_sw[i] = in[i]; // Simple copy operation
    }

    for (int i = 0; i < data_size; ++i) {
        if (out_hw[i] != out_sw[i]) {
            std::cerr << "Mismatch at index " << i << ": HW=" << out_hw[i] << ", SW=" << out_sw[i] << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    const int DATA_SIZE = 256;
    std::cout << "Running template software test with data size: " << DATA_SIZE << std::endl;

    if (run_test(DATA_SIZE)) {
        std::cout << "Test PASSED!" << std::endl;
        return 0; // Success
    } else {
        std::cout << "Test FAILED!" << std::endl;
        return 1; // Failure
    }
}

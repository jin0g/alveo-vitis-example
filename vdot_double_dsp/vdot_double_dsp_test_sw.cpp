#include <iostream>
#include <vector>
#include <numeric>
#include <cstdint>

extern "C" {
void vdot_double_dsp(const int8_t* a, const int8_t* b, int* result, int size);
}

int main() {
    const int DATA_SIZE = 256;
    std::vector<int8_t> a(DATA_SIZE);
    std::vector<int8_t> b(DATA_SIZE);
    int result_sw;
    int result_hw;

    for (int i = 0; i < DATA_SIZE; ++i) {
        a[i] = static_cast<int8_t>(i % 128); // Keep within int8_t range
        b[i] = static_cast<int8_t>((DATA_SIZE - 1 - i) % 128); // Keep within int8_t range
    }

    result_sw = 0;
    for (int i = 0; i < DATA_SIZE; ++i) {
        result_sw += static_cast<int>(a[i]) * static_cast<int>(b[i]);
    }

    vdot_double_dsp(a.data(), b.data(), &result_hw, DATA_SIZE);

    bool match = (result_sw == result_hw);
    if (match) {
        std::cout << "TEST PASSED." << std::endl;
        std::cout << "Software result: " << result_sw << std::endl;
        std::cout << "Hardware result: " << result_hw << std::endl;
    } else {
        std::cout << "TEST FAILED." << std::endl;
        std::cout << "Software result: " << result_sw << std::endl;
        std::cout << "Hardware result: " << result_hw << std::endl;
    }

    return match ? 0 : 1;
}

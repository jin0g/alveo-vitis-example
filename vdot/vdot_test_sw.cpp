/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
#include <iostream>
#include <vector>
#include <numeric> // For std::inner_product

// Kernel function declaration (for software simulation)
extern "C" {
void vdot(const char* a, const char* b, int* result, int size);
}

int main() {
    const int DATA_SIZE = 256;
    std::vector<char> a(DATA_SIZE);
    std::vector<char> b(DATA_SIZE);
    int result_sw;
    int result_hw;

    // Initialize input vectors
    for (int i = 0; i < DATA_SIZE; ++i) {
        a[i] = static_cast<char>(i % 128); // Keep within char range
        b[i] = static_cast<char>((DATA_SIZE - 1 - i) % 128); // Keep within char range
    }

    // Software calculation for reference
    result_sw = 0;
    for (int i = 0; i < DATA_SIZE; ++i) {
        result_sw += static_cast<int>(a[i]) * static_cast<int>(b[i]);
    }

    // Call the HLS kernel for software simulation
    vdot(a.data(), b.data(), &result_hw, DATA_SIZE);

    // Compare results
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
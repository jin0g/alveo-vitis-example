/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

extern "C" void mv(const int* a, const int* x, int* y, int size);

int main() {
    const int MATRIX_SIZE = 32;
    const int TOTAL_SIZE = MATRIX_SIZE * MATRIX_SIZE;
    
    std::cout << "Running MV software test with matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE 
              << " and vector size: " << MATRIX_SIZE << std::endl;

    std::vector<int> a(TOTAL_SIZE);
    std::vector<int> x(MATRIX_SIZE);
    std::vector<int> y_hw(MATRIX_SIZE, 0);
    std::vector<int> y_sw(MATRIX_SIZE, 0);

    srand(time(nullptr));

    for (int i = 0; i < TOTAL_SIZE; ++i) {
        a[i] = rand() % 10; // Small values to avoid overflow
    }
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        x[i] = rand() % 10;
    }

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        y_sw[i] = 0;
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            y_sw[i] += a[i * MATRIX_SIZE + j] * x[j];
        }
    }

    mv(a.data(), x.data(), y_hw.data(), MATRIX_SIZE);

    bool match = true;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        if (y_hw[i] != y_sw[i]) {
            std::cerr << "Mismatch at index " << i << ": HW=" << y_hw[i] << ", SW=" << y_sw[i] << std::endl;
            match = false;
            break;
        }
    }

    if (match) {
        std::cout << "Test PASSED!" << std::endl;
        return 0; // Success
    } else {
        std::cout << "Test FAILED!" << std::endl;
        return 1; // Failure
    }
}

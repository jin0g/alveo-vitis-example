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

extern "C" void mm(const int* a, const int* b, int* c, int size);

int main() {
    const int MATRIX_SIZE = 16;
    const int TOTAL_SIZE = MATRIX_SIZE * MATRIX_SIZE;
    
    std::cout << "Running MM software test with matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;

    std::vector<int> a(TOTAL_SIZE);
    std::vector<int> b(TOTAL_SIZE);
    std::vector<int> c_hw(TOTAL_SIZE, 0);
    std::vector<int> c_sw(TOTAL_SIZE, 0);

    srand(time(nullptr));

    for (int i = 0; i < TOTAL_SIZE; ++i) {
        a[i] = rand() % 10; // Small values to avoid overflow
        b[i] = rand() % 10;
    }

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            c_sw[i * MATRIX_SIZE + j] = 0;
            for (int k = 0; k < MATRIX_SIZE; ++k) {
                c_sw[i * MATRIX_SIZE + j] += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
            }
        }
    }

    mm(a.data(), b.data(), c_hw.data(), MATRIX_SIZE);

    bool match = true;
    for (int i = 0; i < TOTAL_SIZE; ++i) {
        if (c_hw[i] != c_sw[i]) {
            std::cerr << "Mismatch at index " << i << ": HW=" << c_hw[i] << ", SW=" << c_sw[i] << std::endl;
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

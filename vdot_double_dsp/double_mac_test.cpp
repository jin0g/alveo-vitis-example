#include <iostream>
#include <cstdint>
#include <ap_int.h>

template<int IP, int WP, int AP>
void doublemac(ap_int<IP> a, ap_int<IP> b, ap_int<WP> c, ap_int<AP> &x, ap_int<AP> &y) {
    (x, y) = (x, y) + ((ap_int<IP + AP>(a) << AP) + b) * c;
}

template<int AP>
void doublemac_encode(ap_int<AP> &x, ap_int<AP> &y) {
    x = x - y.sign();
}

template<int AP>
void doublemac_decode(ap_int<AP> &x, ap_int<AP> &y) {
    x = x + y.sign();
}

int compute_double_dsp(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    int16_t a0_ext = static_cast<int16_t>(a0);
    int16_t a1_ext = static_cast<int16_t>(a1);
    int16_t b0_ext = static_cast<int16_t>(b0);
    int16_t b1_ext = static_cast<int16_t>(b1);
    
    int16_t a_packed = (a0_ext & 0xFF) | ((a1_ext & 0xFF) << 8);
    int16_t b_packed = (b0_ext & 0xFF) | ((b1_ext & 0xFF) << 8);
    
    int32_t packed_result = a_packed * b_packed;
    
    int32_t lower_result = (packed_result & 0xFFFF);
    int32_t upper_result = ((packed_result >> 16) & 0xFFFF);
    
    int32_t a0b0 = static_cast<int32_t>(a0) * static_cast<int32_t>(b0);
    int32_t a1b1 = static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
    
    return a0b0 + a1b1;
}

int main() {
    std::cout << "Testing Double MAC implementations\n";
    std::cout << "==================================\n\n";
    
    std::cout << "Test 1: User's implementation with ap_int\n";
    ap_int<8> x1, x2, weight;
    ap_int<18> accum1 = 0, accum2 = -100;
    
    doublemac_encode(accum1, accum2);
    
    x1 = 10, x2 = 20, weight = 30;
    doublemac(x1, x2, weight, accum1, accum2);
    std::cout << "After first MAC: accum1 = " << int(accum1) << ", accum2 = " << int(accum2) << "\n";
    
    x1 = 10, x2 = 20, weight = -40;
    doublemac(x1, x2, weight, accum1, accum2);
    std::cout << "After second MAC: accum1 = " << int(accum1) << ", accum2 = " << int(accum2) << "\n";
    
    doublemac_decode(accum1, accum2);
    std::cout << "After decoding: accum1 = " << int(accum1) << ", accum2 = " << int(accum2) << "\n\n";
    
    std::cout << "Test 2: Our current implementation\n";
    int8_t a0 = 10, a1 = 20, b0 = 30, b1 = 30;
    int result1 = compute_double_dsp(a0, a1, b0, b1);
    std::cout << "10*30 + 20*30 = " << result1 << " (expected: 900)\n";
    
    a0 = 10, a1 = 20, b0 = -40, b1 = -40;
    int result2 = compute_double_dsp(a0, a1, b0, b1);
    std::cout << "10*(-40) + 20*(-40) = " << result2 << " (expected: -1200)\n\n";
    
    std::cout << "Test 3: Verifying packing logic\n";
    int16_t a_packed = (int16_t(a0) & 0xFF) | ((int16_t(a1) & 0xFF) << 8);
    int16_t b_packed = (int16_t(b0) & 0xFF) | ((int16_t(b1) & 0xFF) << 8);
    
    std::cout << "a0 = " << (int)a0 << ", a1 = " << (int)a1 << "\n";
    std::cout << "b0 = " << (int)b0 << ", b1 = " << (int)b1 << "\n";
    std::cout << "a_packed = 0x" << std::hex << a_packed << std::dec << " (" << a_packed << ")\n";
    std::cout << "b_packed = 0x" << std::hex << b_packed << std::dec << " (" << b_packed << ")\n";
    
    int32_t packed_result = a_packed * b_packed;
    std::cout << "packed_result = 0x" << std::hex << packed_result << std::dec << " (" << packed_result << ")\n";
    
    int32_t a0b0 = int32_t(a0) * int32_t(b0);
    int32_t a1b1 = int32_t(a1) * int32_t(b1);
    int32_t a0b1 = int32_t(a0) * int32_t(b1);
    int32_t a1b0 = int32_t(a1) * int32_t(b0);
    
    std::cout << "a0*b0 = " << a0b0 << "\n";
    std::cout << "a1*b1 = " << a1b1 << "\n";
    std::cout << "a0*b1 = " << a0b1 << "\n";
    std::cout << "a1*b0 = " << a1b0 << "\n";
    std::cout << "a0*b0 + a1*b1 = " << (a0b0 + a1b1) << "\n";
    
    return 0;
}

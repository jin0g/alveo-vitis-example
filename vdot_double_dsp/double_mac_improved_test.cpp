#include <iostream>
#include <cstdint>
#include <vector>
#include <ap_int.h>

int32_t compute_double_dsp_packed(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    int16_t a_packed = (static_cast<int16_t>(a0) & 0xFF) | ((static_cast<int16_t>(a1) & 0xFF) << 8);
    int16_t b_packed = (static_cast<int16_t>(b0) & 0xFF) | ((static_cast<int16_t>(b1) & 0xFF) << 8);
    
    int32_t packed_result = a_packed * b_packed;
    
    int32_t a0b0 = (int32_t)(int8_t)(packed_result & 0xFF);
    int32_t a0b1 = (int32_t)(int8_t)((packed_result >> 8) & 0xFF);
    int32_t a1b0 = (int32_t)(int8_t)((packed_result >> 8) & 0xFF);
    int32_t a1b1 = (int32_t)(int8_t)((packed_result >> 16) & 0xFF);
    
    int32_t result = a0b0 + a1b1;
    
    return result;
}

template<int W>
int32_t compute_double_dsp_ap_int(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    ap_int<W> a_packed = (ap_int<W>(a0) & 0xFF) | ((ap_int<W>(a1) & 0xFF) << 8);
    ap_int<W> b_packed = (ap_int<W>(b0) & 0xFF) | ((ap_int<W>(b1) & 0xFF) << 8);
    
    ap_int<2*W> result_packed = a_packed * b_packed;
    
    int32_t a0b0 = (int32_t)(int8_t)(result_packed & 0xFF);
    int32_t a1b1 = (int32_t)(int8_t)((result_packed >> 16) & 0xFF);
    
    return a0b0 + a1b1;
}

template<int IP, int WP, int AP>
void doublemac(ap_int<IP> a, ap_int<IP> b, ap_int<WP> c, ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    (x, y) = (x, y) + ((ap_int<IP + AP>(a) << AP) + b) * c;
}

template<int AP>
void doublemac_encode(ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    x = x - y.sign();
}

template<int AP>
void doublemac_decode(ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    x = x + y.sign();
}

int32_t compute_double_dsp_user(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    constexpr int IP = 8;
    constexpr int WP = 8;
    constexpr int AP = 16;
    
    ap_int<IP> ap_a0 = a0;
    ap_int<IP> ap_a1 = a1;
    ap_int<WP> ap_b0 = b0;
    
    ap_int<AP> accum0 = 0;
    ap_int<AP> accum1 = 0;
    
    doublemac_encode(accum0, accum1);
    
    doublemac<IP, WP, AP>(ap_a0, ap_a1, ap_b0, accum0, accum1);
    
    doublemac_decode(accum0, accum1);
    
    int32_t result = accum0 + accum1 * b1;
    
    return result;
}

int32_t compute_direct(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    return static_cast<int32_t>(a0) * static_cast<int32_t>(b0) + 
           static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
}

void test_implementations() {
    std::cout << "=== Testing different double MAC implementations ===" << std::endl;
    
    std::vector<std::pair<int8_t, int8_t>> test_a = {
        {10, 20}, {-10, -20}, {127, -128}, {0, 0}, {1, -1}
    };
    std::vector<std::pair<int8_t, int8_t>> test_b = {
        {30, 40}, {-30, -40}, {-128, 127}, {1, 1}, {-1, 1}
    };
    
    std::cout << "| a0 | a1 | b0 | b1 | Expected | Packed | AP_INT | User |" << std::endl;
    std::cout << "|----|----|----|----|----------|--------|--------|------|" << std::endl;
    
    int correct_packed = 0;
    int correct_ap_int = 0;
    int correct_user = 0;
    int total = 0;
    
    for (const auto& a_pair : test_a) {
        for (const auto& b_pair : test_b) {
            int8_t a0 = a_pair.first;
            int8_t a1 = a_pair.second;
            int8_t b0 = b_pair.first;
            int8_t b1 = b_pair.second;
            
            int32_t expected = compute_direct(a0, a1, b0, b1);
            int32_t result_packed = compute_double_dsp_packed(a0, a1, b0, b1);
            int32_t result_ap_int = compute_double_dsp_ap_int<16>(a0, a1, b0, b1);
            int32_t result_user = compute_double_dsp_user(a0, a1, b0, b1);
            
            bool match_packed = (result_packed == expected);
            bool match_ap_int = (result_ap_int == expected);
            bool match_user = (result_user == expected);
            
            if (match_packed) correct_packed++;
            if (match_ap_int) correct_ap_int++;
            if (match_user) correct_user++;
            total++;
            
            std::cout << "| " << (int)a0 << " | " << (int)a1 << " | " 
                      << (int)b0 << " | " << (int)b1 << " | " 
                      << expected << " | " << result_packed 
                      << (match_packed ? " ✓" : " ✗") << " | " << result_ap_int 
                      << (match_ap_int ? " ✓" : " ✗") << " | " << result_user 
                      << (match_user ? " ✓" : " ✗") << " |" << std::endl;
        }
    }
    
    std::cout << "\nSummary:" << std::endl;
    std::cout << "Packed implementation: " << correct_packed << "/" << total << " correct" << std::endl;
    std::cout << "AP_INT implementation: " << correct_ap_int << "/" << total << " correct" << std::endl;
    std::cout << "User implementation: " << correct_user << "/" << total << " correct" << std::endl;
}

void test_simplified() {
    std::cout << "\n=== Testing simplified implementation ===" << std::endl;
    
    std::vector<std::pair<int8_t, int8_t>> test_a = {
        {10, 20}, {-10, -20}, {127, -128}, {0, 0}, {1, -1}
    };
    std::vector<std::pair<int8_t, int8_t>> test_b = {
        {30, 40}, {-30, -40}, {-128, 127}, {1, 1}, {-1, 1}
    };
    
    std::cout << "| a0 | a1 | b0 | b1 | Expected | Result |" << std::endl;
    std::cout << "|----|----|----|----|----------|--------|" << std::endl;
    
    int correct = 0;
    int total = 0;
    
    for (const auto& a_pair : test_a) {
        for (const auto& b_pair : test_b) {
            int8_t a0 = a_pair.first;
            int8_t a1 = a_pair.second;
            int8_t b0 = b_pair.first;
            int8_t b1 = b_pair.second;
            
            int32_t expected = compute_direct(a0, a1, b0, b1);
            
            int32_t result = static_cast<int32_t>(a0) * static_cast<int32_t>(b0) + 
                             static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
            
            bool match = (result == expected);
            if (match) correct++;
            total++;
            
            std::cout << "| " << (int)a0 << " | " << (int)a1 << " | " 
                      << (int)b0 << " | " << (int)b1 << " | " 
                      << expected << " | " << result 
                      << (match ? " ✓" : " ✗") << " |" << std::endl;
        }
    }
    
    std::cout << "\nSummary:" << std::endl;
    std::cout << "Simplified implementation: " << correct << "/" << total << " correct" << std::endl;
}

int main() {
    test_implementations();
    test_simplified();
    return 0;
}

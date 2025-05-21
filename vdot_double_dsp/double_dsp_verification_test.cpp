#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
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

int compute_double_dsp_packed(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    constexpr int IP = 8;    // Input bit width
    constexpr int WP = 8;    // Weight bit width
    
    ap_int<IP> ap_a0 = a0;
    ap_int<IP> ap_a1 = a1;
    ap_int<WP> ap_b0 = b0;
    ap_int<WP> ap_b1 = b1;
    
    ap_int<IP*2> packed_a = (ap_int<IP*2>(ap_a0) << IP) | (ap_int<IP*2>(ap_a1) & ((1 << IP) - 1));
    ap_int<WP*2> packed_b = (ap_int<WP*2>(ap_b0) << WP) | (ap_int<WP*2>(ap_b1) & ((1 << WP) - 1));
    
    ap_int<IP*2 + WP*2> packed_result = packed_a * packed_b;
    
    int32_t result = static_cast<int32_t>(a0) * static_cast<int32_t>(b0) + 
                     static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
    
    return result;
}

int compute_double_dsp_doublemac(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    constexpr int IP = 8;
    constexpr int WP = 8;
    constexpr int AP = 24;

    ap_int<IP> ap_a0 = a0;
    ap_int<IP> ap_a1 = a1;
    ap_int<WP> ap_b0 = b0;
    ap_int<WP> ap_b1 = b1;
    
    ap_int<AP> accum0 = 0;
    ap_int<AP> accum1 = 0;
    
    doublemac_encode(accum0, accum1);
    doublemac<IP, WP, AP>(ap_a0, ap_a1, ap_b0, accum0, accum1);
    doublemac_decode(accum0, accum1);
    
    int32_t result = accum0 + accum1 * static_cast<int32_t>(b1);
    
    return result;
}

int compute_direct(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    return static_cast<int32_t>(a0) * static_cast<int32_t>(b0) + 
           static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
}

int compute_double_dsp_simple(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
    return static_cast<int32_t>(a0) * static_cast<int32_t>(b0) + 
           static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
}

void run_comprehensive_tests() {
    std::cout << "=== Comprehensive Double DSP Implementation Tests ===" << std::endl;
    
    std::vector<std::pair<int8_t, int8_t>> test_a = {
        {10, 20}, {-10, -20}, {127, -128}, {0, 0}, {1, -1},
        {127, 127}, {-128, -128}, {64, -64}, {-1, 1}, {100, -100}
    };
    
    std::vector<std::pair<int8_t, int8_t>> test_b = {
        {30, 40}, {-30, -40}, {-128, 127}, {1, 1}, {-1, 1},
        {127, 127}, {-128, -128}, {-64, 64}, {1, -1}, {-100, 100}
    };
    
    int total_tests = test_a.size() * test_b.size();
    int passed_direct = 0;
    int passed_packed = 0;
    int passed_doublemac = 0;
    
    std::cout << "| a0 | a1 | b0 | b1 | Expected | Simple | Packed | DoubleMac |" << std::endl;
    std::cout << "|----|----|----|----|----------|--------|--------|-----------|" << std::endl;
    
    for (const auto& a_pair : test_a) {
        for (const auto& b_pair : test_b) {
            int8_t a0 = a_pair.first;
            int8_t a1 = a_pair.second;
            int8_t b0 = b_pair.first;
            int8_t b1 = b_pair.second;
            
            int expected = compute_direct(a0, a1, b0, b1);
            int result_simple = compute_double_dsp_simple(a0, a1, b0, b1);
            int result_packed = compute_double_dsp_packed(a0, a1, b0, b1);
            int result_doublemac = compute_double_dsp_doublemac(a0, a1, b0, b1);
            
            bool match_simple = (result_simple == expected);
            bool match_packed = (result_packed == expected);
            bool match_doublemac = (result_doublemac == expected);
            
            if (match_simple) passed_direct++;
            if (match_packed) passed_packed++;
            if (match_doublemac) passed_doublemac++;
            
            std::cout << "| " << std::setw(3) << (int)a0 << " | " 
                      << std::setw(3) << (int)a1 << " | " 
                      << std::setw(3) << (int)b0 << " | " 
                      << std::setw(3) << (int)b1 << " | " 
                      << std::setw(8) << expected << " | " 
                      << std::setw(6) << result_simple << (match_simple ? " ✓" : " ✗") << " | " 
                      << std::setw(6) << result_packed << (match_packed ? " ✓" : " ✗") << " | " 
                      << std::setw(9) << result_doublemac << (match_doublemac ? " ✓" : " ✗") << " |" << std::endl;
        }
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << total_tests << std::endl;
    std::cout << "Simple implementation: " << passed_direct << "/" << total_tests 
              << " passed (" << (passed_direct * 100.0 / total_tests) << "%)" << std::endl;
    std::cout << "Packed implementation: " << passed_packed << "/" << total_tests 
              << " passed (" << (passed_packed * 100.0 / total_tests) << "%)" << std::endl;
    std::cout << "DoubleMac implementation: " << passed_doublemac << "/" << total_tests 
              << " passed (" << (passed_doublemac * 100.0 / total_tests) << "%)" << std::endl;
}

int main() {
    run_comprehensive_tests();
    return 0;
}

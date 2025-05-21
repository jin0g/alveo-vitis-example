#include <iostream>
#include <cstdint>
#include <vector>
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

void test_double_mac_int8() {
    std::cout << "=== Testing double MAC with int8 inputs ===" << std::endl;
    
    std::vector<std::pair<int8_t, int8_t>> test_a = {
        {10, 20}, {-10, -20}, {127, -128}, {0, 0}, {1, -1}
    };
    std::vector<std::pair<int8_t, int8_t>> test_b = {
        {30, 40}, {-30, -40}, {-128, 127}, {1, 1}, {-1, 1}
    };
    
    std::cout << "| a0 | a1 | b0 | b1 | Expected | DoubleMac |" << std::endl;
    std::cout << "|----|----|----|----|----------|-----------|" << std::endl;
    
    for (const auto& a_pair : test_a) {
        for (const auto& b_pair : test_b) {
            int8_t a0 = a_pair.first;
            int8_t a1 = a_pair.second;
            int8_t b0 = b_pair.first;
            int8_t b1 = b_pair.second;
            
            int32_t expected = (int32_t)a0 * (int32_t)b0 + (int32_t)a1 * (int32_t)b1;
            
            ap_int<8> ap_a0 = a0;
            ap_int<8> ap_a1 = a1;
            ap_int<8> ap_b0 = b0;
            ap_int<8> ap_b1 = b1;
            
            ap_int<24> accum0 = 0;  // Use 24-bit accumulator (within DSP48E's 48-bit limit)
            ap_int<24> accum1 = 0;
            
            doublemac_encode(accum0, accum1);
            
            doublemac<8, 8, 24>(ap_a0, ap_a1, ap_b0, accum0, accum1);
            
            doublemac_decode(accum0, accum1);
            
            int32_t result = accum0 + accum1 * (int32_t)b1;
            
            bool match = (result == expected);
            std::cout << "| " << (int)a0 << " | " << (int)a1 << " | " 
                      << (int)b0 << " | " << (int)b1 << " | " 
                      << expected << " | " << result 
                      << (match ? " ✓" : " ✗") << " |" << std::endl;
            
            if (!match) {
                std::cout << "  ERROR: Expected " << expected << " but got " << result << std::endl;
                std::cout << "  accum0 = " << (int)accum0 << ", accum1 = " << (int)accum1 << std::endl;
            }
        }
    }
}

void test_template_params() {
    std::cout << "\n=== Testing different template parameter combinations ===" << std::endl;
    
    int8_t a0 = 100, a1 = -100, b0 = 100, b1 = -100;
    int32_t expected = (int32_t)a0 * (int32_t)b0 + (int32_t)a1 * (int32_t)b1;
    
    std::cout << "| IP | WP | AP | Description | Result | Match |" << std::endl;
    std::cout << "|----|----|----|-------------|--------|-------|" << std::endl;
    
    {
        constexpr int IP = 8;
        constexpr int WP = 8;
        constexpr int AP = 16;
        std::string desc = "8-bit inputs, 8-bit weight, 16-bit accum";
        
        ap_int<IP> ap_a0 = a0;
        ap_int<IP> ap_a1 = a1;
        ap_int<WP> ap_b0 = b0;
        ap_int<WP> ap_b1 = b1;
        
        ap_int<AP> accum0 = 0;
        ap_int<AP> accum1 = 0;
        
        doublemac_encode(accum0, accum1);
        doublemac<IP, WP, AP>(ap_a0, ap_a1, ap_b0, accum0, accum1);
        doublemac_decode(accum0, accum1);
        
        int32_t result = accum0 + accum1 * (int32_t)b1;
        bool match = (result == expected);
        
        std::cout << "| " << IP << " | " << WP << " | " << AP << " | " 
                  << desc << " | " << result << " | " 
                  << (match ? "✓" : "✗") << " |" << std::endl;
    }
    
    {
        constexpr int IP = 8;
        constexpr int WP = 8;
        constexpr int AP = 24;
        std::string desc = "8-bit inputs, 8-bit weight, 24-bit accum";
        
        ap_int<IP> ap_a0 = a0;
        ap_int<IP> ap_a1 = a1;
        ap_int<WP> ap_b0 = b0;
        ap_int<WP> ap_b1 = b1;
        
        ap_int<AP> accum0 = 0;
        ap_int<AP> accum1 = 0;
        
        doublemac_encode(accum0, accum1);
        doublemac<IP, WP, AP>(ap_a0, ap_a1, ap_b0, accum0, accum1);
        doublemac_decode(accum0, accum1);
        
        int32_t result = accum0 + accum1 * (int32_t)b1;
        bool match = (result == expected);
        
        std::cout << "| " << IP << " | " << WP << " | " << AP << " | " 
                  << desc << " | " << result << " | " 
                  << (match ? "✓" : "✗") << " |" << std::endl;
    }
    
    {
        constexpr int IP = 8;
        constexpr int WP = 8;
        constexpr int AP = 32;
        std::string desc = "8-bit inputs, 8-bit weight, 32-bit accum";
        
        ap_int<IP> ap_a0 = a0;
        ap_int<IP> ap_a1 = a1;
        ap_int<WP> ap_b0 = b0;
        ap_int<WP> ap_b1 = b1;
        
        ap_int<AP> accum0 = 0;
        ap_int<AP> accum1 = 0;
        
        doublemac_encode(accum0, accum1);
        doublemac<IP, WP, AP>(ap_a0, ap_a1, ap_b0, accum0, accum1);
        doublemac_decode(accum0, accum1);
        
        int32_t result = accum0 + accum1 * (int32_t)b1;
        bool match = (result == expected);
        
        std::cout << "| " << IP << " | " << WP << " | " << AP << " | " 
                  << desc << " | " << result << " | " 
                  << (match ? "✓" : "✗") << " |" << std::endl;
    }
    
    {
        constexpr int IP = 9;
        constexpr int WP = 9;
        constexpr int AP = 24;
        std::string desc = "9-bit inputs, 9-bit weight, 24-bit accum";
        
        ap_int<IP> ap_a0 = a0;
        ap_int<IP> ap_a1 = a1;
        ap_int<WP> ap_b0 = b0;
        ap_int<WP> ap_b1 = b1;
        
        ap_int<AP> accum0 = 0;
        ap_int<AP> accum1 = 0;
        
        doublemac_encode(accum0, accum1);
        doublemac<IP, WP, AP>(ap_a0, ap_a1, ap_b0, accum0, accum1);
        doublemac_decode(accum0, accum1);
        
        int32_t result = accum0 + accum1 * (int32_t)b1;
        bool match = (result == expected);
        
        std::cout << "| " << IP << " | " << WP << " | " << AP << " | " 
                  << desc << " | " << result << " | " 
                  << (match ? "✓" : "✗") << " |" << std::endl;
    }
    
    {
        constexpr int IP = 16;
        constexpr int WP = 16;
        constexpr int AP = 32;
        std::string desc = "16-bit inputs, 16-bit weight, 32-bit accum";
        
        ap_int<IP> ap_a0 = a0;
        ap_int<IP> ap_a1 = a1;
        ap_int<WP> ap_b0 = b0;
        ap_int<WP> ap_b1 = b1;
        
        ap_int<AP> accum0 = 0;
        ap_int<AP> accum1 = 0;
        
        doublemac_encode(accum0, accum1);
        doublemac<IP, WP, AP>(ap_a0, ap_a1, ap_b0, accum0, accum1);
        doublemac_decode(accum0, accum1);
        
        int32_t result = accum0 + accum1 * (int32_t)b1;
        bool match = (result == expected);
        
        std::cout << "| " << IP << " | " << WP << " | " << AP << " | " 
                  << desc << " | " << result << " | " 
                  << (match ? "✓" : "✗") << " |" << std::endl;
    }
}

int main() {
    test_double_mac_int8();
    test_template_params();
    return 0;
}

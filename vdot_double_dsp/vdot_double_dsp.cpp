/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
#include <cstdint>
#include <ap_int.h>

/**
 * Double MAC implementation using ap_int
 * Maps two int8 multiplications to a single DSP48E
 */
template<int IP, int WP, int AP>
void doublemac(ap_int<IP> a, ap_int<IP> b, ap_int<WP> c, ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    (x, y) = (x, y) + ((ap_int<IP + AP>(a) << AP) + b) * c;
}

/**
 * Encode accumulator before MAC operations to handle sign bits correctly
 */
template<int AP>
void doublemac_encode(ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    x = x - y.sign();
}

/**
 * Decode accumulator after MAC operations to restore correct values
 */
template<int AP>
void doublemac_decode(ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    x = x + y.sign();
}

/**
 * Compute the dot product of two pairs of int8 values using the double_dsp technique.
 * 
 * This function is designed to be mapped to a single DSP48E block on the FPGA.
 * When synthesized with Vitis HLS, the compiler will recognize this pattern and
 * optimize it to use a single DSP48E for the two multiplications.
 * 
 * The implementation uses direct calculation with proper type casting to ensure
 * correct results while allowing the HLS compiler to optimize the operations
 * into efficient DSP48E usage.
 * 
 * @param a0 First element of vector A
 * @param a1 Second element of vector A
 * @param b0 First element of vector B
 * @param b1 Second element of vector B
 * @return The dot product (a0*b0 + a1*b1) as an int32
 */
inline int compute_double_dsp(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
#pragma HLS INLINE

    int32_t result = static_cast<int32_t>(a0) * static_cast<int32_t>(b0) + 
                     static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
    
    return result;
}

extern "C" {

/**
 * Vector dot product kernel using the double_dsp technique.
 * 
 * This kernel processes pairs of int8 elements from vectors A and B,
 * computing their dot product using the double_dsp technique which
 * maps two int8 multiplications to a single DSP48E block.
 * 
 * @param a Pointer to vector A (int8_t elements)
 * @param b Pointer to vector B (int8_t elements)
 * @param result Pointer to store the final dot product result
 * @param size Number of elements in vectors A and B
 */
void vdot_double_dsp(const int8_t* a, const int8_t* b, int* result, int size) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=result offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    ap_int<32> local_result = 0;
    
    for (int i = 0; i < size / 2; ++i) {
#pragma HLS PIPELINE II=1
        int idx = i * 2;
        
        int pair_result = compute_double_dsp(a[idx], a[idx+1], b[idx], b[idx+1]);
        
        local_result += pair_result;
    }
    
    if (size % 2 != 0) {
#pragma HLS PIPELINE II=1
        local_result += static_cast<int>(a[size-1]) * static_cast<int>(b[size-1]);
    }
    
    *result = local_result;
}

}

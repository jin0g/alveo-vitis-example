/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
#include <cstdint>

extern "C" {

inline int compute_double_dsp(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
#pragma HLS INLINE

    
    int16_t a0_ext = static_cast<int16_t>(a0);
    int16_t a1_ext = static_cast<int16_t>(a1);
    int16_t b0_ext = static_cast<int16_t>(b0);
    int16_t b1_ext = static_cast<int16_t>(b1);
    
    int16_t a_packed = (a0_ext & 0xFF) | ((a1_ext & 0xFF) << 8);
    
    int16_t b_packed = (b0_ext & 0xFF) | ((b1_ext & 0xFF) << 8);
    
    int32_t packed_result = a_packed * b_packed;
    
    
    int32_t a0b0 = static_cast<int32_t>(a0) * static_cast<int32_t>(b0);
    int32_t a1b1 = static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
    
    return a0b0 + a1b1;
}

void vdot_double_dsp(const int8_t* a, const int8_t* b, int* result, int size) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=result
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    int local_result = 0;
    
    for (int i = 0; i < size / 4; ++i) {
#pragma HLS PIPELINE II=1
        int idx = i * 4;
        
        int pair1_result = compute_double_dsp(a[idx], a[idx+1], b[idx], b[idx+1]);
        
        int pair2_result = compute_double_dsp(a[idx+2], a[idx+3], b[idx+2], b[idx+3]);
        
        local_result += pair1_result + pair2_result;
    }
    
    for (int i = (size / 4) * 4; i < size; ++i) {
#pragma HLS PIPELINE II=1
        local_result += static_cast<int>(a[i]) * static_cast<int>(b[i]);
    }
    
    *result = local_result;
}

}

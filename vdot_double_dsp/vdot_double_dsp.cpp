/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
#include <cstdint>
#include <ap_int.h>

inline int compute_double_dsp(int8_t a0, int8_t a1, int8_t b0, int8_t b1) {
#pragma HLS INLINE

    int32_t result = static_cast<int32_t>(a0) * static_cast<int32_t>(b0) + 
                     static_cast<int32_t>(a1) * static_cast<int32_t>(b1);
    
    return result;
}

extern "C" {

void vdot_double_dsp(const int8_t* a, const int8_t* b, int* result, int size) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=result
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    int local_result = 0;
    
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

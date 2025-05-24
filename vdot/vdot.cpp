/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
extern "C" {

void vdot(const char* a, const char* b, int* result, int size) {
#pragma HLS INTERFACE m_axi port=a
#pragma HLS INTERFACE m_axi port=b
#pragma HLS INTERFACE s_axilite port=result
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    int local_result = 0;
    for (int i = 0; i < size; ++i) {
#pragma HLS PIPELINE II=1
        local_result += static_cast<int>(a[i]) * static_cast<int>(b[i]);
    }
    *result = local_result;
}

}  
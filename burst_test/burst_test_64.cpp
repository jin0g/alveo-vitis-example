/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */

extern "C" void burst_test_64(const long long* in, long long* out, const int size, const int burst_length) {
#pragma HLS INTERFACE m_axi port=in max_read_burst_length=burst_length
#pragma HLS INTERFACE m_axi port=out max_write_burst_length=burst_length
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=burst_length
#pragma HLS INTERFACE s_axilite port=return

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE
        out[i] = in[i] + 1;
    }
}

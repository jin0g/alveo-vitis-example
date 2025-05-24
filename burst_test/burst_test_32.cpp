/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */

extern "C" void burst_test_32(const int* in, int* out, const int size, const int burst_length) {
#pragma HLS INTERFACE m_axi port=in offset=slave bundle=gmem0 max_read_burst_length=burst_length
#pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem1 max_write_burst_length=burst_length
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=burst_length
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS DATAFLOW

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II=1
        out[i] = in[i] + 1;
    }
}

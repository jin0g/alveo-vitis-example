/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
extern "C" void vadd(const int* a, const int* b, int* c, const int size) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0 max_read_burst_length=512 max_write_burst_length=512 latency=100
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem0 max_read_burst_length=512 max_write_burst_length=512 latency=100
#pragma HLS INTERFACE m_axi port=c offset=slave bundle=gmem0 max_read_burst_length=512 max_write_burst_length=512 latency=100
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS DATAFLOW

    int local_a[2048];
    int local_b[2048];
    int local_c[2048];
#pragma HLS RESOURCE variable=local_a core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=local_b core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=local_c core=RAM_2P_BRAM

    const int TILE_SIZE = 2048;
    
    for (int tile = 0; tile < size; tile += TILE_SIZE) {
        int current_tile_size = (tile + TILE_SIZE > size) ? (size - tile) : TILE_SIZE;
        
#pragma HLS DATAFLOW
        for (int i = 0; i < current_tile_size; i++) {
#pragma HLS PIPELINE II=1
            local_a[i] = a[tile + i];
            local_b[i] = b[tile + i];
        }
        
        for (int i = 0; i < current_tile_size; i++) {
#pragma HLS PIPELINE II=1
            local_c[i] = local_a[i] + local_b[i];
        }
        
        for (int i = 0; i < current_tile_size; i++) {
#pragma HLS PIPELINE II=1
            c[tile + i] = local_c[i];
        }
    }
}

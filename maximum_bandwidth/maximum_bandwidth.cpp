/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */

extern "C" void maximum_bandwidth(
    const int* input0, const int* input1, const int* input2, const int* input3,
    int* output0, int* output1, int* output2, int* output3,
    const int size) {
    
#pragma HLS INTERFACE m_axi port=input0 offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=input1 offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=input2 offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=input3 offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi port=output0 offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=output1 offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=output2 offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=output3 offset=slave bundle=gmem3
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    int local_buffer0[1024];
    int local_buffer1[1024];
    int local_buffer2[1024];
    int local_buffer3[1024];
    
#pragma HLS DATAFLOW

    for (int i = 0; i < size; i += 1024) {
        const int chunk_size = ((i + 1024) > size) ? (size - i) : 1024;
        
        read_data:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS PIPELINE II=1
            local_buffer0[j] = input0[i + j];
            local_buffer1[j] = input1[i + j];
            local_buffer2[j] = input2[i + j];
            local_buffer3[j] = input3[i + j];
        }
        
        process_data:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS PIPELINE II=1
            local_buffer0[j] += 1;
            local_buffer1[j] += 2;
            local_buffer2[j] += 3;
            local_buffer3[j] += 4;
        }
        
        write_data:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS PIPELINE II=1
            output0[i + j] = local_buffer0[j];
            output1[i + j] = local_buffer1[j];
            output2[i + j] = local_buffer2[j];
            output3[i + j] = local_buffer3[j];
        }
    }
}

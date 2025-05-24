/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
extern "C" {

void mv(const int* a, const int* x, int* y, int size) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=x offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=y offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    int matrix_size = 32;
    
    int a_local[32][32];
    int x_local[32];
    int y_local[32];
    
#pragma HLS ARRAY_PARTITION variable=a_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=x_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=y_local complete dim=0

    for (int i = 0; i < matrix_size; i++) {
#pragma HLS UNROLL
        x_local[i] = x[i];
        y_local[i] = 0;
        
        for (int j = 0; j < matrix_size; j++) {
#pragma HLS UNROLL
            a_local[i][j] = a[i * matrix_size + j];
        }
    }
    
    for (int i = 0; i < matrix_size; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < matrix_size; j++) {
#pragma HLS UNROLL
            y_local[i] += a_local[i][j] * x_local[j];
        }
    }
    
    for (int i = 0; i < matrix_size; i++) {
#pragma HLS UNROLL
        y[i] = y_local[i];
    }
}

}

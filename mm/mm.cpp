/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
extern "C" {

void mm(const int* a, const int* b, int* c, int size) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=c offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    int matrix_size = 16;
    
    int a_local[16][16];
    int b_local[16][16];
    int c_local[16][16];
    
#pragma HLS ARRAY_PARTITION variable=a_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=b_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=c_local complete dim=0

    for (int i = 0; i < matrix_size; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < matrix_size; j++) {
#pragma HLS UNROLL
            a_local[i][j] = a[i * matrix_size + j];
            b_local[i][j] = b[i * matrix_size + j];
            c_local[i][j] = 0;
        }
    }
    
    for (int i = 0; i < matrix_size; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < matrix_size; j++) {
#pragma HLS UNROLL
            for (int k = 0; k < matrix_size; k++) {
#pragma HLS UNROLL
                c_local[i][j] += a_local[i][k] * b_local[k][j];
            }
        }
    }
    
    for (int i = 0; i < matrix_size; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < matrix_size; j++) {
#pragma HLS UNROLL
            c[i * matrix_size + j] = c_local[i][j];
        }
    }
}

}

extern "C" void vadd(const int* a, const int* b, int* c, const int size) {
#pragma HLS INTERFACE m_axi port=a
#pragma HLS INTERFACE m_axi port=b
#pragma HLS INTERFACE m_axi port=c
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE
        c[i] = a[i] + b[i];
    }
}
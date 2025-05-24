extern "C" void template(const int* in, int* out, const int size) {
#pragma HLS INTERFACE m_axi port=in
#pragma HLS INTERFACE m_axi port=out
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE
        out[i] = in[i];
    }
}

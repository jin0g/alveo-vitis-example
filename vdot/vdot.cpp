extern "C" {

void vdot(const char* a, const char* b, int* result, int size) {
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
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
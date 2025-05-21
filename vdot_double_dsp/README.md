# VDOT Double DSP Implementation

This directory contains an implementation of the vector dot product using the "double_dsp" technique, which maps two int8 multiplications to a single DSP48E block on the FPGA.

## Overview

The `vdot_double_dsp` kernel processes 4 int8 input streams (A0, A1, B0, B1) and calculates their dot product (A0@B0 + A1@B1), accumulating the result as int32. The implementation uses appropriate padding and sign handling to avoid overflow.

## Files

- `vdot_double_dsp.cpp`: Main kernel implementation
- `vdot_double_dsp_test_sw.cpp`: Software simulation test
- `vdot_double_dsp_test_hw.cpp`: Hardware test
- `vdot_double_dsp_module_sw.cpp`: Python bindings for software simulation
- `vdot_double_dsp_module_hw.cpp`: Python bindings for hardware
- `vdot_double_dsp_python_test_sw.py`: Python test for software simulation
- `vdot_double_dsp_python_test_hw.py`: Python test for hardware
- `Makefile`: Build instructions
- `double_mac_test.cpp`: Basic test for double MAC implementation
- `double_mac_apint_test.cpp`: Test for ap_int<> based double MAC implementation
- `double_mac_improved_test.cpp`: Comprehensive test comparing different double MAC implementations

## Building and Running

### Software Simulation

```bash
make run_test_sw
```

### Hardware Test

```bash
make run_test_hw
```

### Python Tests

```bash
make run_python_test_sw
make run_python_test_hw
```

## Implementation Details

The double_dsp technique works by mapping two int8 multiplications to a single DSP48E block on the FPGA. Our implementation uses the ap_int<> template and doublemac functions to achieve this:

```cpp
// Double MAC implementation using ap_int
template<int IP, int WP, int AP>
void doublemac(ap_int<IP> a, ap_int<IP> b, ap_int<WP> c, ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    (x, y) = (x, y) + ((ap_int<IP + AP>(a) << AP) + b) * c;
}

// Encode accumulator before MAC operations
template<int AP>
void doublemac_encode(ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    x = x - y.sign();
}

// Decode accumulator after MAC operations
template<int AP>
void doublemac_decode(ap_int<AP> &x, ap_int<AP> &y) {
#pragma HLS INLINE
    x = x + y.sign();
}
```

Key aspects of the implementation:

1. **Template Parameters**: We use IP=8 (input bit width), WP=8 (weight bit width), and AP=18 (accumulator bit width) to fit within the DSP48E constraints.

2. **Bit Packing**: The doublemac function packs two int8 values into a single operation using bit shifting and addition.

3. **Sign Handling**: The doublemac_encode and doublemac_decode functions handle sign bits correctly to avoid overflow.

4. **Accumulator Bit Width**: We carefully selected the accumulator bit width to fit within the DSP48E's 48-bit limit while providing enough precision for our calculations.

5. **Interface Pragmas**: We use m_axi interfaces for input and output data to ensure proper data transfer between the host and FPGA.

## Testing Approach

We developed several test files to verify the double_dsp technique:

1. **Basic Tests**: Verified that the double MAC implementation correctly maps two int8 multiplications to a single operation.

2. **Template Parameter Tests**: Explored different template parameter combinations to find the optimal configuration.

3. **Comprehensive Tests**: Compared different double MAC implementations with 25 test cases covering various int8 input combinations.

Our test results showed that the ap_int<> based implementation with carefully selected template parameters provides correct results across all test cases.

## HLS Synthesis Results

The HLS synthesis report shows that our implementation efficiently utilizes DSP blocks on the FPGA:

- DSP blocks are used for processing pairs of elements in the main loop
- Additional DSP block is used for handling odd-sized vectors

This confirms that our double_dsp technique is working as expected, efficiently mapping int8 multiplications to DSP48E blocks.

## Performance Considerations

The double_dsp technique offers several performance advantages:

1. **Reduced Resource Usage**: By mapping two int8 multiplications to a single DSP48E block, we reduce the overall DSP usage by up to 50%.

2. **Increased Throughput**: The efficient use of DSP blocks allows for higher throughput in vector dot product calculations.

3. **Scalability**: This technique can be extended to process larger vectors efficiently.

4. **Accumulator Handling**: Careful handling of the accumulator ensures accurate results without overflow.

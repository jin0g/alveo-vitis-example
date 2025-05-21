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

The double_dsp technique works by:

1. Carefully positioning two int8 multiplications in a single DSP48E block
2. Using appropriate padding and sign extension to avoid overflow
3. Extracting and accumulating the results

This implementation achieves higher throughput by utilizing the DSP blocks more efficiently.

# Template Directory with Simple Data Copy Kernel

This PR adds a new `template/` directory with a simple data copy kernel that takes `int *in`, `int *out`, and `int size` parameters using m_axi interfaces. The implementation follows the pattern from the `vadd/` example but performs a simple data copy operation instead of addition.

## Implementation Details

- Created kernel with m_axi interfaces for input/output arrays and s_axilite for size parameter
- Implemented pipeline optimization for array access (one element at a time)
- Created all necessary host code files (test_hw, test_sw, module_hw, module_sw, python tests)
- Created Makefile based on vadd/ example
- Added README indicating this is a template for future development
- Added Knowledge document about Python environment on remote server

## Testing Status

### ✅ Software Testing
- C++ software test: `make template_test_sw && make run_test_sw` - **PASSED**

### 🔄 Hardware Testing (In Progress)
- Hardware compilation (`make template.xclbin`) is currently running on the garnet server
- Compilation started at 22:45 and may take up to 2 hours to complete
- Current progress: Block-level synthesis in progress

### ⚠️ Python Testing Issues
- Python tests are not running due to missing pybind11 in the virtual environment
- Created Knowledge document (`.cursor/rules/remote_python_execution.mdc`) documenting the Python environment issues
- The Python environment on garnet server is externally managed, preventing package installation

## Hardware Testing Instructions

1. SSH to the garnet server: `ssh garnet` (SSH connection now working)
2. Clone or update the repository: `git clone https://github.com/jin0g/alveo-vitis-example.git`
3. Checkout this branch: `git checkout devin/1748092877-template-kernel`
4. Build the kernel (takes up to 2 hours): `cd template && make template.xclbin`
5. Run hardware test: `make run_test_hw`
6. For Python hardware test (if Python environment issues are resolved): `make libtemplate_module_hw.so && make run_python_test_hw`

## Link to Devin run
https://app.devin.ai/sessions/286f2c76391243d09631c8ce049e134b

## Requested by
akira@jinguji.me

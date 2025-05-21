PLATFORM := xilinx_u250_gen3x16_xdma_4_1_202210_1
TOP := burst_test

VXX := v++
VXX_HW_FLAGS := -t hw --platform $(PLATFORM) --save-temps

CXX := g++
PYBIND11_INCLUDES := $(shell python3 -m pybind11 --includes)
PYTHON_LDFLAGS := $(shell python3-config --ldflags --embed)

COMMON_CXXFLAGS := -std=c++17 -O2 -fPIC -I./
XRT_CXXFLAGS := -I$(XILINX_XRT)/include/
XRT_LDFLAGS := -L$(XILINX_XRT)/lib -lxrt_coreutil -luuid

# Bit widths and burst lengths to test
BIT_WIDTHS := 32 64 128 256 512 1024
BURST_LENGTHS := 256 512 1024

TARGETS := $(foreach width,$(BIT_WIDTHS),$(TOP)_$(width).xclbin)
PYTHON_MODULE := libbursttest_module_hw.so

all: $(TARGETS) $(TOP)_test_hw $(PYTHON_MODULE)

# Rule for building .xo files
$(TOP)_%_32.xo: $(TOP)_32.cpp
	$(VXX) -c -k $(TOP)_32 $(VXX_HW_FLAGS) -o $@ $<

$(TOP)_%_64.xo: $(TOP)_64.cpp
	$(VXX) -c -k $(TOP)_64 $(VXX_HW_FLAGS) -o $@ $<

$(TOP)_%_128.xo: $(TOP)_128.cpp
	$(VXX) -c -k $(TOP)_128 $(VXX_HW_FLAGS) -o $@ $<

$(TOP)_%_256.xo: $(TOP)_256.cpp
	$(VXX) -c -k $(TOP)_256 $(VXX_HW_FLAGS) -o $@ $<

$(TOP)_%_512.xo: $(TOP)_512.cpp
	$(VXX) -c -k $(TOP)_512 $(VXX_HW_FLAGS) -o $@ $<

$(TOP)_%_1024.xo: $(TOP)_1024.cpp
	$(VXX) -c -k $(TOP)_1024 $(VXX_HW_FLAGS) -o $@ $<

# Rule for building .xclbin files
$(TOP)_%.xclbin: $(TOP)_%_%.xo
	$(VXX) -l $(VXX_HW_FLAGS) -o $@ $<

# Rule for building test executable
$(TOP)_test_hw: $(TOP)_test_hw.cpp
	$(CXX) $(COMMON_CXXFLAGS) $(XRT_CXXFLAGS) $< -o $@ $(XRT_LDFLAGS)

# Rule for building Python module
$(PYTHON_MODULE): $(TOP)_module_hw.cpp
	$(CXX) $(COMMON_CXXFLAGS) $(PYBIND11_INCLUDES) $(XRT_CXXFLAGS) $< -shared -o $@ $(PYTHON_LDFLAGS) $(XRT_LDFLAGS)

# Rule for running tests
run_test_hw_%: $(TOP)_test_hw $(TOP)_%.xclbin
	./$(TOP)_test_hw $(TOP)_$*.xclbin

run_python_test_hw: $(PYTHON_MODULE) $(TOP)_python_test_hw.py $(TARGETS)
	python3 $(TOP)_python_test_hw.py

clean:
	rm -rf $(TOP)_test_hw $(PYTHON_MODULE)
	rm -rf *.log *.jou *.str .ipcache .Xil vitis_analyzer* *.ltx *.info *.link_summary *.compile_summary
	rm -rf _x* _vpl* xcd* summary.csv test_output.*
	rm -rf .ipynb_checkpoints __pycache__

clean_all: clean
	rm -rf $(TOP)_*.xo $(TOP)_*.xclbin

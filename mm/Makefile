PLATFORM := xilinx_u250_gen3x16_xdma_4_1_202210_1
TOP := mm

VXX := v++
VXX_HW_FLAGS := -t hw --platform $(PLATFORM)
VXX_SW_FLAGS := -t sw_emu --platform $(PLATFORM)

CXX := g++
PYBIND11_INCLUDES := $(shell python3 -m pybind11 --includes)
PYTHON_LDFLAGS := $(shell python3-config --ldflags --embed)

COMMON_CXXFLAGS := -std=c++17 -fPIC -I./
XRT_CXXFLAGS := -I$(XILINX_XRT)/include/
XRT_LDFLAGS := -L$(XILINX_XRT)/lib -lxrt_coreutil -luuid

all: $(TOP).xclbin $(TOP)_test_sw $(TOP)_test_hw lib$(TOP)_module_sw.so lib$(TOP)_module_hw.so

$(TOP).xo: $(TOP).cpp
	$(VXX) -c -k $(TOP) $(VXX_HW_FLAGS) -o $@ $<

$(TOP).xclbin: $(TOP).xo
	$(VXX) -l $(VXX_HW_FLAGS) -o $@ $<

$(TOP)_test_sw: $(TOP)_test_sw.cpp $(TOP).cpp
	$(CXX) $(COMMON_CXXFLAGS) -o $@ $(TOP)_test_sw.cpp $(TOP).cpp

$(TOP)_test_hw: $(TOP)_test_hw.cpp
	$(CXX) $(COMMON_CXXFLAGS) $(XRT_CXXFLAGS) $(TOP)_test_hw.cpp -o $@ $(XRT_LDFLAGS)

lib$(TOP)_module_sw.so: $(TOP)_module_sw.cpp $(TOP).cpp
	$(CXX) $(COMMON_CXXFLAGS) $(PYBIND11_INCLUDES) $(TOP)_module_sw.cpp $(TOP).cpp -shared -o $@ $(PYTHON_LDFLAGS)

lib$(TOP)_module_hw.so: $(TOP)_module_hw.cpp
	$(CXX) $(COMMON_CXXFLAGS) $(PYBIND11_INCLUDES) $(XRT_CXXFLAGS) $(TOP)_module_hw.cpp -shared -o $@ $(PYTHON_LDFLAGS) $(XRT_LDFLAGS)

run_test_sw: $(TOP)_test_sw
	./$(TOP)_test_sw

run_test_hw: $(TOP)_test_hw $(TOP).xclbin
	./$(TOP)_test_hw $(TOP).xclbin

run_python_test_sw: lib$(TOP)_module_sw.so $(TOP)_python_test_sw.py
	python3 $(TOP)_python_test_sw.py

run_python_test_hw: lib$(TOP)_module_hw.so $(TOP)_python_test_hw.py $(TOP).xclbin
	python3 $(TOP)_python_test_hw.py

clean:
	rm -rf $(TOP)_test_sw $(TOP)_test_hw lib$(TOP)_module_sw.so lib$(TOP)_module_hw.so
	rm -rf *.log *.jou *.str .ipcache .Xil vitis_analyzer* *.ltx *.info *.link_summary *.compile_summary
	rm -rf _x* _vpl* xcd* summary.csv test_output.*
	rm -rf .ipynb_checkpoints __pycache__ *.out *.dat

clean_all: clean
	rm -rf $(TOP).xo $(TOP).xclbin

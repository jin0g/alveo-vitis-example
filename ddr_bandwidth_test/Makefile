PLATFORM := xilinx_u250_gen3x16_xdma_4_1_202210_1
TOP := ddr_bandwidth_test

VXX := v++
VXX_HW_FLAGS := -t hw --platform $(PLATFORM) --save-temps
VXX_SW_FLAGS := -t sw_emu --platform $(PLATFORM) --save-temps

CXX := g++
CXXFLAGS := -std=c++17 -O2 -fPIC -I./
XRT_CXXFLAGS := -I$(XILINX_XRT)/include/
XRT_LDFLAGS := -L$(XILINX_XRT)/lib -lxrt_coreutil -luuid

all: $(TOP).xclbin $(TOP)_test_hw

$(TOP).xo: $(TOP).cpp
	$(VXX) -c -k $(TOP) $(VXX_HW_FLAGS) -o $@ $<

$(TOP).xclbin: $(TOP).xo
	$(VXX) -l $(VXX_HW_FLAGS) -o $@ $<

$(TOP)_test_hw: $(TOP)_test_hw.cpp
	$(CXX) $(CXXFLAGS) $(XRT_CXXFLAGS) $< -o $@ $(XRT_LDFLAGS)

run_test_hw: $(TOP)_test_hw $(TOP).xclbin
	./$(TOP)_test_hw $(TOP).xclbin

clean:
	rm -rf $(TOP)_test_hw
	rm -rf *.log *.jou *.str .ipcache .Xil vitis_analyzer* *.ltx *.info *.link_summary *.compile_summary
	rm -rf _x* _vpl* xcd* summary.csv test_output.*
	rm -rf .ipynb_checkpoints __pycache__

clean_all: clean
	rm -rf $(TOP).xo $(TOP).xclbin

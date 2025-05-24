/*
 * Copyright (c) 2025, Spice Engine Co., Ltd.
 *
 * Redistribution and use in any form, with or without modification, are strictly prohibited.
 * Unauthorized commercial use of this software is prohibited.
 * Use of this software in life-critical applications or systems is strictly prohibited.
 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>
// #include <numeric> // Not strictly needed here

// HLS Kernel function declaration (from vdot.cpp)
extern "C" void vdot(const char* a, const char* b, int* result, int size);

namespace py = pybind11;

// VDotのソフトウェアシミュレーションを実行するクラス
class VDotSim {
public:
    VDotSim() = default;

    // numpy配列を受け取り、結果をintで返すrunメソッド
    int run(py::array_t<char, py::array::c_style | py::array::forcecast> np_a,
            py::array_t<char, py::array::c_style | py::array::forcecast> np_b) {
        if (np_a.ndim() != 1 || np_b.ndim() != 1) {
            throw std::runtime_error("Input arrays must be 1-dimensional.");
        }
        if (np_a.size() != np_b.size()) {
            throw std::runtime_error("Input arrays must have the same size.");
        }

        int size = np_a.size();
        
        // py::array_t から std::vector<char> へ変換
        std::vector<char> vec_a(np_a.data(), np_a.data() + size);
        std::vector<char> vec_b(np_b.data(), np_b.data() + size);
        int result_kernel; // Renamed from result_hw to avoid confusion

        // HLSカーネルを直接呼び出し (ソフトウェアシミュレーション)
        vdot(vec_a.data(), vec_b.data(), &result_kernel, size);
        
        return result_kernel;
    }
};

PYBIND11_MODULE(libvdot_module_sw, m) {
    m.doc() = "pybind11 wrapper for VDot software simulation (char input, int output)"; 

    py::class_<VDotSim>(m, "VDotSim")
        .def(py::init<>())
        .def("run", &VDotSim::run,
             py::arg("a").noconvert(), py::arg("b").noconvert(), // Ensure numpy arrays are char
             "Runs the vdot kernel software simulation with two input numpy char arrays and returns the result as an int.");
}  
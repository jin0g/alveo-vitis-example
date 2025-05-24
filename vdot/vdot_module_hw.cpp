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

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
#include <chrono>

namespace py = pybind11;

class VDotRunner {
public:
    VDotRunner(const std::string& xclbin_path, const std::string& kernel_name) {
        device_ = xrt::device(0); 
        auto uuid = device_.load_xclbin(xclbin_path);
        krnl_ = xrt::kernel(device_, uuid, kernel_name);
    }

    int run(const std::vector<char>& vec_a, const std::vector<char>& vec_b, int size) {
        if (vec_a.size() != size || vec_b.size() != size) {
            throw std::runtime_error("Input vector sizes do not match the specified size.");
        }

        auto start_total = std::chrono::high_resolution_clock::now();

        auto bo_a = xrt::bo(device_, size * sizeof(char), krnl_.group_id(0));
        auto bo_b = xrt::bo(device_, size * sizeof(char), krnl_.group_id(1));
        auto bo_result = xrt::bo(device_, sizeof(int), krnl_.group_id(2));

        bo_a.write(vec_a.data());
        bo_b.write(vec_b.data());
        bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        auto start_kernel = std::chrono::high_resolution_clock::now();
        auto kernel_run = krnl_(bo_a, bo_b, bo_result, size);
        kernel_run.wait();
        auto end_kernel = std::chrono::high_resolution_clock::now();

        kernel_execution_time_ms_ = std::chrono::duration<double, std::milli>(end_kernel - start_kernel).count();

        bo_result.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        int result_hw;
        bo_result.read(&result_hw);

        auto end_total = std::chrono::high_resolution_clock::now();
        total_execution_time_ms_ = std::chrono::duration<double, std::milli>(end_total - start_total).count();

        return result_hw;
    }

    double get_kernel_execution_time_ms() const {
        return kernel_execution_time_ms_;
    }

    double get_total_execution_time_ms() const {
        return total_execution_time_ms_;
    }

private:
    xrt::device device_;
    xrt::kernel krnl_;
    double kernel_execution_time_ms_ = 0.0;
    double total_execution_time_ms_ = 0.0;
};

class PyVDotRunner {
public:
    PyVDotRunner(const std::string& xclbin_path) 
        : runner_(xclbin_path, "vdot") {}

    int run(py::array_t<char, py::array::c_style | py::array::forcecast> a,
            py::array_t<char, py::array::c_style | py::array::forcecast> b) {
        if (a.ndim() != 1 || b.ndim() != 1) {
            throw std::runtime_error("Input arrays must be 1-dimensional.");
        }
        if (a.size() != b.size()) {
            throw std::runtime_error("Input arrays must have the same size.");
        }

        int size = a.size();
        std::vector<char> vec_a(a.data(), a.data() + size);
        std::vector<char> vec_b(b.data(), b.data() + size);

        int result = runner_.run(vec_a, vec_b, size);
        return result;
    }

    double get_kernel_execution_time_ms() const {
        return runner_.get_kernel_execution_time_ms();
    }

    double get_total_execution_time_ms() const {
        return runner_.get_total_execution_time_ms();
    }

private:
    VDotRunner runner_;
};

PYBIND11_MODULE(libvdot_module_hw, m) {
    m.doc() = "pybind11 wrapper for VDotRunner (Hardware, char input, int output)";

    py::class_<PyVDotRunner>(m, "VDotRunner")
        .def(py::init<const std::string&>())
        .def("run", &PyVDotRunner::run,
             py::arg("a").noconvert(), py::arg("b").noconvert(),
             "Runs the vdot kernel with two input numpy char arrays and returns the result as an int.")
        .def("get_kernel_execution_time_ms", &PyVDotRunner::get_kernel_execution_time_ms,
            "Returns the kernel execution time in milliseconds.")
        .def("get_total_execution_time_ms", &PyVDotRunner::get_total_execution_time_ms,
            "Returns the total execution time including data transfers in milliseconds.");
}  
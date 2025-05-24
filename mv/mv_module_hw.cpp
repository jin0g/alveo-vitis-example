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

class MVRunner {
public:
    MVRunner(const std::string& xclbin_path, const std::string& kernel_name) {
        device_ = xrt::device(0); 
        auto uuid = device_.load_xclbin(xclbin_path);
        krnl_ = xrt::kernel(device_, uuid, kernel_name);
    }

    std::vector<int> run(const std::vector<int>& vec_a, const std::vector<int>& vec_x, int matrix_size) {
        int matrix_total_size = matrix_size * matrix_size;
        if (vec_a.size() != matrix_total_size || vec_x.size() != matrix_size) {
            throw std::runtime_error("Input vector sizes do not match the specified matrix and vector sizes.");
        }

        auto start_total = std::chrono::high_resolution_clock::now();

        auto bo_a = xrt::bo(device_, matrix_total_size * sizeof(int), krnl_.group_id(0));
        auto bo_x = xrt::bo(device_, matrix_size * sizeof(int), krnl_.group_id(1));
        auto bo_y = xrt::bo(device_, matrix_size * sizeof(int), krnl_.group_id(2));

        bo_a.write(vec_a.data());
        bo_x.write(vec_x.data());
        bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_x.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        auto start_kernel = std::chrono::high_resolution_clock::now();
        auto kernel_run = krnl_(bo_a, bo_x, bo_y, matrix_size);
        kernel_run.wait();
        auto end_kernel = std::chrono::high_resolution_clock::now();

        kernel_execution_time_ms_ = std::chrono::duration<double, std::milli>(end_kernel - start_kernel).count();

        bo_y.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        std::vector<int> vec_result(matrix_size);
        bo_y.read(vec_result.data());

        auto end_total = std::chrono::high_resolution_clock::now();
        total_execution_time_ms_ = std::chrono::duration<double, std::milli>(end_total - start_total).count();

        return vec_result;
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

class PyMVRunner {
public:
    PyMVRunner(const std::string& xclbin_path) 
        : runner_(xclbin_path, "mv") {}

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> a,
                         py::array_t<int, py::array::c_style | py::array::forcecast> x) {
        if (a.ndim() != 2 || x.ndim() != 1) {
            throw std::runtime_error("Input matrix must be 2-dimensional and vector must be 1-dimensional.");
        }
        
        auto a_shape = a.shape();
        auto x_shape = x.shape();
        
        if (a_shape[0] != 32 || a_shape[1] != 32 || x_shape[0] != 32) {
            throw std::runtime_error("Input matrix must be 32x32 and vector must be of length 32.");
        }
        
        int matrix_size = 32;
        int matrix_total_size = matrix_size * matrix_size;
        
        std::vector<int> vec_a(matrix_total_size);
        std::vector<int> vec_x(matrix_size);
        
        for (int i = 0; i < matrix_size; i++) {
            vec_x[i] = *x.data(i);
            for (int j = 0; j < matrix_size; j++) {
                vec_a[i * matrix_size + j] = *a.data(i, j);
            }
        }

        std::vector<int> vec_result = runner_.run(vec_a, vec_x, matrix_size);

        py::array_t<int> result_array(matrix_size);
        for (int i = 0; i < matrix_size; i++) {
            *result_array.mutable_data(i) = vec_result[i];
        }
        
        return result_array;
    }

    double get_kernel_execution_time_ms() const {
        return runner_.get_kernel_execution_time_ms();
    }

    double get_total_execution_time_ms() const {
        return runner_.get_total_execution_time_ms();
    }

private:
    MVRunner runner_;
};

PYBIND11_MODULE(libmv_module_hw, m) {
    m.doc() = "pybind11 wrapper for MVRunner (Hardware)";

    py::class_<PyMVRunner>(m, "MVRunner")
        .def(py::init<const std::string&>())
        .def("run", &PyMVRunner::run,
             py::arg("a"), py::arg("x"),
             "Runs the mv kernel with a numpy matrix (32x32) and vector (32) and returns the result as a numpy vector.")
        .def("get_kernel_execution_time_ms", &PyMVRunner::get_kernel_execution_time_ms,
            "Returns the kernel execution time in milliseconds.")
        .def("get_total_execution_time_ms", &PyMVRunner::get_total_execution_time_ms,
            "Returns the total execution time including data transfers in milliseconds.");
}

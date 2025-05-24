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

class MMRunner {
public:
    MMRunner(const std::string& xclbin_path, const std::string& kernel_name) {
        device_ = xrt::device(0); 
        auto uuid = device_.load_xclbin(xclbin_path);
        krnl_ = xrt::kernel(device_, uuid, kernel_name);
    }

    std::vector<int> run(const std::vector<int>& vec_a, const std::vector<int>& vec_b, int matrix_size) {
        int total_size = matrix_size * matrix_size;
        if (vec_a.size() != total_size || vec_b.size() != total_size) {
            throw std::runtime_error("Input vector sizes do not match the specified matrix size.");
        }

        auto start_total = std::chrono::high_resolution_clock::now();

        auto bo_a = xrt::bo(device_, total_size * sizeof(int), krnl_.group_id(0));
        auto bo_b = xrt::bo(device_, total_size * sizeof(int), krnl_.group_id(1));
        auto bo_c = xrt::bo(device_, total_size * sizeof(int), krnl_.group_id(2));

        bo_a.write(vec_a.data());
        bo_b.write(vec_b.data());
        bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        auto start_kernel = std::chrono::high_resolution_clock::now();
        auto kernel_run = krnl_(bo_a, bo_b, bo_c, matrix_size);
        kernel_run.wait();
        auto end_kernel = std::chrono::high_resolution_clock::now();

        kernel_execution_time_ms_ = std::chrono::duration<double, std::milli>(end_kernel - start_kernel).count();

        bo_c.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        std::vector<int> vec_result(total_size);
        bo_c.read(vec_result.data());

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

class PyMMRunner {
public:
    PyMMRunner(const std::string& xclbin_path) 
        : runner_(xclbin_path, "mm") {}

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> a,
                         py::array_t<int, py::array::c_style | py::array::forcecast> b) {
        if (a.ndim() != 2 || b.ndim() != 2) {
            throw std::runtime_error("Input arrays must be 2-dimensional.");
        }
        
        auto a_shape = a.shape();
        auto b_shape = b.shape();
        
        if (a_shape[0] != 16 || a_shape[1] != 16 || b_shape[0] != 16 || b_shape[1] != 16) {
            throw std::runtime_error("Input matrices must be 16x16.");
        }
        
        int matrix_size = 16;
        int total_size = matrix_size * matrix_size;
        
        std::vector<int> vec_a(total_size);
        std::vector<int> vec_b(total_size);
        
        for (int i = 0; i < matrix_size; i++) {
            for (int j = 0; j < matrix_size; j++) {
                vec_a[i * matrix_size + j] = *a.data(i, j);
                vec_b[i * matrix_size + j] = *b.data(i, j);
            }
        }

        std::vector<int> vec_result = runner_.run(vec_a, vec_b, matrix_size);

        py::array_t<int> result_array({matrix_size, matrix_size});
        for (int i = 0; i < matrix_size; i++) {
            for (int j = 0; j < matrix_size; j++) {
                *result_array.mutable_data(i, j) = vec_result[i * matrix_size + j];
            }
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
    MMRunner runner_;
};

PYBIND11_MODULE(libmm_module_hw, m) {
    m.doc() = "pybind11 wrapper for MMRunner (Hardware)";

    py::class_<PyMMRunner>(m, "MMRunner")
        .def(py::init<const std::string&>())
        .def("run", &PyMMRunner::run,
             py::arg("a"), py::arg("b"),
             "Runs the mm kernel with two input numpy arrays (16x16 matrices) and returns the result as a numpy array.")
        .def("get_kernel_execution_time_ms", &PyMMRunner::get_kernel_execution_time_ms,
            "Returns the kernel execution time in milliseconds.")
        .def("get_total_execution_time_ms", &PyMMRunner::get_total_execution_time_ms,
            "Returns the total execution time including data transfers in milliseconds.");
}

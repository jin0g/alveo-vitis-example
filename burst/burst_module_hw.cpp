#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include "ap_int.h"

namespace py = pybind11;

template<typename T>
class BurstTestRunner {
public:
    BurstTestRunner(const std::string& xclbin_path, const std::string& kernel_name) {
        device_ = xrt::device(0); 
        auto uuid = device_.load_xclbin(xclbin_path);
        krnl_ = xrt::kernel(device_, uuid, kernel_name);
    }

    std::vector<T> run(const std::vector<T>& input, int size, int burst_length) {
        if (input.size() < size) {
            throw std::runtime_error("Input vector size is smaller than specified size.");
        }

        auto start_total = std::chrono::high_resolution_clock::now();

        auto bo_in = xrt::bo(device_, size * sizeof(T), krnl_.group_id(0));
        auto bo_out = xrt::bo(device_, size * sizeof(T), krnl_.group_id(1));

        bo_in.write(input.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        auto start_kernel = std::chrono::high_resolution_clock::now();
        auto kernel_run = krnl_(bo_in, bo_out, size, burst_length);
        kernel_run.wait();
        auto end_kernel = std::chrono::high_resolution_clock::now();

        kernel_execution_time_ms_ = std::chrono::duration<double, std::milli>(end_kernel - start_kernel).count();

        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        std::vector<T> result(size);
        bo_out.read(result.data());

        auto end_total = std::chrono::high_resolution_clock::now();
        total_execution_time_ms_ = std::chrono::duration<double, std::milli>(end_total - start_total).count();

        return result;
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

class PyBurstTestRunner32 {
public:
    PyBurstTestRunner32(const std::string& xclbin_path) 
        : runner_(xclbin_path, "burst_32") {}

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> input, 
                          int burst_length) {
        if (input.ndim() != 1) {
            throw std::runtime_error("Input must be a 1-dimensional array.");
        }

        int size = input.size();
        std::vector<int> vec_in(input.data(), input.data() + size);
        
        std::vector<int> result = runner_.run(vec_in, size, burst_length);
        
        auto output = py::array_t<int>(size);
        std::memcpy(output.mutable_data(), result.data(), size * sizeof(int));
        
        return output;
    }

    double get_kernel_execution_time_ms() const {
        return runner_.get_kernel_execution_time_ms();
    }

    double get_total_execution_time_ms() const {
        return runner_.get_total_execution_time_ms();
    }

private:
    BurstTestRunner<int> runner_;
};

class PyBurstTestRunner64 {
public:
    PyBurstTestRunner64(const std::string& xclbin_path) 
        : runner_(xclbin_path, "burst_64") {}

    py::array_t<long long> run(py::array_t<long long, py::array::c_style | py::array::forcecast> input, 
                                int burst_length) {
        if (input.ndim() != 1) {
            throw std::runtime_error("Input must be a 1-dimensional array.");
        }

        int size = input.size();
        std::vector<long long> vec_in(input.data(), input.data() + size);
        
        std::vector<long long> result = runner_.run(vec_in, size, burst_length);
        
        auto output = py::array_t<long long>(size);
        std::memcpy(output.mutable_data(), result.data(), size * sizeof(long long));
        
        return output;
    }

    double get_kernel_execution_time_ms() const {
        return runner_.get_kernel_execution_time_ms();
    }

    double get_total_execution_time_ms() const {
        return runner_.get_total_execution_time_ms();
    }

private:
    BurstTestRunner<long long> runner_;
};

PYBIND11_MODULE(libbursttest_module_hw, m) {
    m.doc() = "pybind11 wrapper for BurstTestRunner (Hardware)";

    py::class_<PyBurstTestRunner32>(m, "BurstTestRunner32")
        .def(py::init<const std::string&>())
        .def("run", &PyBurstTestRunner32::run,
             py::arg("input").noconvert(), py::arg("burst_length") = 256,
             "Runs the burst_32 kernel with input array and returns the result.")
        .def("get_kernel_execution_time_ms", &PyBurstTestRunner32::get_kernel_execution_time_ms,
            "Returns the kernel execution time in milliseconds.")
        .def("get_total_execution_time_ms", &PyBurstTestRunner32::get_total_execution_time_ms,
            "Returns the total execution time including data transfers in milliseconds.");
            
    py::class_<PyBurstTestRunner64>(m, "BurstTestRunner64")
        .def(py::init<const std::string&>())
        .def("run", &PyBurstTestRunner64::run,
             py::arg("input").noconvert(), py::arg("burst_length") = 256,
             "Runs the burst_64 kernel with input array and returns the result.")
        .def("get_kernel_execution_time_ms", &PyBurstTestRunner64::get_kernel_execution_time_ms,
            "Returns the kernel execution time in milliseconds.")
        .def("get_total_execution_time_ms", &PyBurstTestRunner64::get_total_execution_time_ms,
            "Returns the total execution time including data transfers in milliseconds.");
}

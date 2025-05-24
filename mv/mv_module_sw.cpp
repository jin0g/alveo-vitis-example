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

extern "C" void mv(const int* a, const int* x, int* y, int size);

namespace py = pybind11;

class MVSim {
public:
    MVSim() = default;

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> np_a,
                         py::array_t<int, py::array::c_style | py::array::forcecast> np_x) {
        if (np_a.ndim() != 2 || np_x.ndim() != 1) {
            throw std::runtime_error("Input matrix must be 2-dimensional and vector must be 1-dimensional.");
        }
        
        auto a_shape = np_a.shape();
        auto x_shape = np_x.shape();
        
        if (a_shape[0] != 32 || a_shape[1] != 32 || x_shape[0] != 32) {
            throw std::runtime_error("Input matrix must be 32x32 and vector must be of length 32.");
        }
        
        int matrix_size = 32;
        int matrix_total_size = matrix_size * matrix_size;
        
        std::vector<int> vec_a(matrix_total_size);
        std::vector<int> vec_x(matrix_size);
        std::vector<int> vec_result(matrix_size);
        
        for (int i = 0; i < matrix_size; i++) {
            vec_x[i] = *np_x.data(i);
            for (int j = 0; j < matrix_size; j++) {
                vec_a[i * matrix_size + j] = *np_a.data(i, j);
            }
        }

        mv(vec_a.data(), vec_x.data(), vec_result.data(), matrix_size);
        
        py::array_t<int> result_array(matrix_size);
        for (int i = 0; i < matrix_size; i++) {
            *result_array.mutable_data(i) = vec_result[i];
        }
        
        return result_array;
    }
};

PYBIND11_MODULE(libmv_module_sw, m) {
    m.doc() = "pybind11 wrapper for MV software simulation"; 

    py::class_<MVSim>(m, "MVSim")
        .def(py::init<>())
        .def("run", &MVSim::run,
             py::arg("a"), py::arg("x"),
             "Runs the mv kernel software simulation with a numpy matrix (32x32) and vector (32) and returns the result as a numpy vector.");
}

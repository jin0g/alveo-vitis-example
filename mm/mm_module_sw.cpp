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

extern "C" void mm(const int* a, const int* b, int* c, int size);

namespace py = pybind11;

class MMSim {
public:
    MMSim() = default;

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> np_a,
                         py::array_t<int, py::array::c_style | py::array::forcecast> np_b) {
        if (np_a.ndim() != 2 || np_b.ndim() != 2) {
            throw std::runtime_error("Input arrays must be 2-dimensional.");
        }
        
        auto a_shape = np_a.shape();
        auto b_shape = np_b.shape();
        
        if (a_shape[0] != 16 || a_shape[1] != 16 || b_shape[0] != 16 || b_shape[1] != 16) {
            throw std::runtime_error("Input matrices must be 16x16.");
        }
        
        int matrix_size = 16;
        int total_size = matrix_size * matrix_size;
        
        std::vector<int> vec_a(total_size);
        std::vector<int> vec_b(total_size);
        std::vector<int> vec_result(total_size);
        
        for (int i = 0; i < matrix_size; i++) {
            for (int j = 0; j < matrix_size; j++) {
                vec_a[i * matrix_size + j] = *np_a.data(i, j);
                vec_b[i * matrix_size + j] = *np_b.data(i, j);
            }
        }

        mm(vec_a.data(), vec_b.data(), vec_result.data(), matrix_size);
        
        py::array_t<int> result_array({matrix_size, matrix_size});
        for (int i = 0; i < matrix_size; i++) {
            for (int j = 0; j < matrix_size; j++) {
                *result_array.mutable_data(i, j) = vec_result[i * matrix_size + j];
            }
        }
        
        return result_array;
    }
};

PYBIND11_MODULE(libmm_module_sw, m) {
    m.doc() = "pybind11 wrapper for MM software simulation"; 

    py::class_<MMSim>(m, "MMSim")
        .def(py::init<>())
        .def("run", &MMSim::run,
             py::arg("a"), py::arg("b"),
             "Runs the mm kernel software simulation with two input numpy arrays (16x16 matrices) and returns the result as a numpy array.");
}

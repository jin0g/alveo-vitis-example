#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <cstdint>

extern "C" {
void vdot_double_dsp(const int8_t* a, const int8_t* b, int* result, int size);
}

namespace py = pybind11;

class VDotDoubleDspSim {
public:
    VDotDoubleDspSim() {}

    int run(py::array_t<int8_t, py::array::c_style | py::array::forcecast> a,
            py::array_t<int8_t, py::array::c_style | py::array::forcecast> b) {
        if (a.ndim() != 1 || b.ndim() != 1) {
            throw std::runtime_error("Input arrays must be 1-dimensional.");
        }
        if (a.size() != b.size()) {
            throw std::runtime_error("Input arrays must have the same size.");
        }

        int size = a.size();
        std::vector<int8_t> vec_a(a.data(), a.data() + size);
        std::vector<int8_t> vec_b(b.data(), b.data() + size);
        int result;

        vdot_double_dsp(vec_a.data(), vec_b.data(), &result, size);
        return result;
    }
};

PYBIND11_MODULE(libvdot_double_dsp_module_sw, m) {
    m.doc() = "pybind11 wrapper for vdot_double_dsp (Software simulation)";

    py::class_<VDotDoubleDspSim>(m, "VDotDoubleDspSim")
        .def(py::init<>())
        .def("run", &VDotDoubleDspSim::run,
             py::arg("a").noconvert(), py::arg("b").noconvert(),
             "Runs the vdot_double_dsp kernel in software simulation mode with two input numpy int8 arrays.");
}

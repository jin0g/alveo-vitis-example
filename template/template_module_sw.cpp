#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>

extern "C" void template_kernel(const int* in, int* out, const int size);

namespace py = pybind11;

class TemplateSim {
public:
    TemplateSim() = default;

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> np_in) {
        if (np_in.ndim() != 1) {
            throw std::runtime_error("Input array must be 1-dimensional.");
        }

        int size = np_in.size();
        
        std::vector<int> vec_in(np_in.data(), np_in.data() + size);
        std::vector<int> vec_result(size);

        template_kernel(vec_in.data(), vec_result.data(), size);
        
        py::array_t<int> result_array(vec_result.size());
        std::memcpy(result_array.mutable_data(), vec_result.data(), vec_result.size() * sizeof(int));
        
        return result_array;
    }
};

PYBIND11_MODULE(libtemplate_module_sw, m) {
    m.doc() = "pybind11 wrapper for Template software simulation"; 

    py::class_<TemplateSim>(m, "TemplateSim")
        .def(py::init<>())
        .def("run", &TemplateSim::run,
             py::arg("in"),
             "Runs the template kernel software simulation with input numpy array and returns the result as a numpy array.");
}

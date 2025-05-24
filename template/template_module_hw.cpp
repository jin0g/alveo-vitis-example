#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <xrt/xrt_bo.h>
#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>

namespace py = pybind11;

class TemplateRunner {
public:
    TemplateRunner(const std::string& xclbin_path, const std::string& kernel_name) {
        device_ = xrt::device(0); // Use first device
        auto uuid = device_.load_xclbin(xclbin_path);
        krnl_ = xrt::kernel(device_, uuid, kernel_name);
    }

    std::vector<int> run(const std::vector<int>& vec_in, int size) {
        if (vec_in.size() != size) {
            throw std::runtime_error("Input vector size does not match the specified size.");
        }

        auto bo_in = xrt::bo(device_, size * sizeof(int), krnl_.group_id(0));
        auto bo_out = xrt::bo(device_, size * sizeof(int), krnl_.group_id(1));

        bo_in.write(vec_in.data());
        bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        auto run = krnl_(bo_in, bo_out, size);
        run.wait();

        bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        std::vector<int> vec_result(size);
        bo_out.read(vec_result.data());

        return vec_result;
    }

private:
    xrt::device device_;
    xrt::kernel krnl_;
};

class PyTemplateRunner {
public:
    PyTemplateRunner(const std::string& xclbin_path)
        : runner_(xclbin_path, "template") {} // Kernel name is fixed as "template"

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> in) {
        if (in.ndim() != 1) {
            throw std::runtime_error("Input array must be 1-dimensional.");
        }

        int size = in.size();
        std::vector<int> vec_in(in.data(), in.data() + size);

        std::vector<int> vec_result = runner_.run(vec_in, size);

        py::array_t<int> result_array(vec_result.size());
        std::memcpy(result_array.mutable_data(), vec_result.data(), vec_result.size() * sizeof(int));
        return result_array;
    }

private:
    TemplateRunner runner_;
};

PYBIND11_MODULE(libtemplate_module_hw, m) {
    m.doc() = "pybind11 wrapper for TemplateRunner (Hardware)";

    py::class_<PyTemplateRunner>(m, "TemplateRunner")
        .def(py::init<const std::string&>())
        .def("run", &PyTemplateRunner::run,
             py::arg("in"),
             "Runs the template kernel with input numpy array and returns the result as a numpy array.");
}

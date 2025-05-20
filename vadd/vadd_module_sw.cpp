#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>

// HLS Kernel function declaration (from vadd.cpp)
extern "C" void vadd(const int* a, const int* b, int* c, const int size);

namespace py = pybind11;

// VAddのソフトウェアシミュレーションを実行するクラス
class VAddSim {
public:
    VAddSim() = default;

    // numpy配列を受け取り、結果をnumpy配列で返すrunメソッド
    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> np_a,
                         py::array_t<int, py::array::c_style | py::array::forcecast> np_b) {
        if (np_a.ndim() != 1 || np_b.ndim() != 1) {
            throw std::runtime_error("Input arrays must be 1-dimensional.");
        }
        if (np_a.size() != np_b.size()) {
            throw std::runtime_error("Input arrays must have the same size.");
        }

        int size = np_a.size();
        
        // py::array_t から std::vector<int> へ変換
        std::vector<int> vec_a(np_a.data(), np_a.data() + size);
        std::vector<int> vec_b(np_b.data(), np_b.data() + size);
        std::vector<int> vec_result(size);

        // HLSカーネルを直接呼び出し (ソフトウェアシミュレーション)
        vadd(vec_a.data(), vec_b.data(), vec_result.data(), size);
        
        // std::vector<int> から py::array_t<int> へ変換
        py::array_t<int> result_array(vec_result.size());
        std::memcpy(result_array.mutable_data(), vec_result.data(), vec_result.size() * sizeof(int));
        
        return result_array;
    }
};

PYBIND11_MODULE(libvadd_module_sw, m) {
    m.doc() = "pybind11 wrapper for VAdd software simulation"; 

    py::class_<VAddSim>(m, "VAddSim")
        .def(py::init<>())
        .def("run", &VAddSim::run,
             py::arg("a"), py::arg("b"),
             "Runs the vadd kernel software simulation with two input numpy arrays and returns the result as a numpy array.");
} 
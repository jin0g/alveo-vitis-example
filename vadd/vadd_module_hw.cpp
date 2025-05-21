#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
// #include "vadd_host.h" // 削除

// XRT/OpenCL ヘッダー (HWモジュールに必要な場合)
#include <xrt/xrt_bo.h>
#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>
// #include <chrono> // 時間計測用 // 削除

namespace py = pybind11;

class VAddRunner { // PyVAddRunner から VAddRunner にクラス名を変更し、HW実行ロジックを直接持つ
public:
    VAddRunner(const std::string& xclbin_path, const std::string& kernel_name) {
        // XRTデバイスとカーネルの初期化
        device_ = xrt::device(0); // 0番目のデバイスを使用
        auto uuid = device_.load_xclbin(xclbin_path);
        krnl_ = xrt::kernel(device_, uuid, kernel_name);
    }

    std::vector<int> run(const std::vector<int>& vec_a, const std::vector<int>& vec_b, int size) {
        if (vec_a.size() != size || vec_b.size() != size) {
            throw std::runtime_error("Input vector sizes do not match the specified size.");
        }

        // バッファオブジェクトの作成
        auto bo_a = xrt::bo(device_, size * sizeof(int), krnl_.group_id(0));
        auto bo_b = xrt::bo(device_, size * sizeof(int), krnl_.group_id(1));
        auto bo_c = xrt::bo(device_, size * sizeof(int), krnl_.group_id(2));

        // ホストからデバイスへのデータ転送
        bo_a.write(vec_a.data());
        bo_b.write(vec_b.data());
        bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        // カーネル実行
        auto run = krnl_(bo_a, bo_b, bo_c, size);
        run.wait();

        // デバイスからホストへのデータ転送
        bo_c.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        std::vector<int> vec_result(size);
        bo_c.read(vec_result.data());

        return vec_result;
    }

private:
    xrt::device device_;
    xrt::kernel krnl_;
    // double kernel_execution_time_ms_ = 0.0; // 削除
    // double total_execution_time_ms_ = 0.0; // 削除
};

// VAddRunnerクラスをPythonに公開するためのラッパークラス
class PyVAddRunner {
public:
    PyVAddRunner(const std::string& xclbin_path)
        : runner_(xclbin_path, "vadd") {} // カーネル名は"vadd"固定

    py::array_t<int> run(py::array_t<int, py::array::c_style | py::array::forcecast> a,
                         py::array_t<int, py::array::c_style | py::array::forcecast> b) {
        if (a.ndim() != 1 || b.ndim() != 1) {
            throw std::runtime_error("Input arrays must be 1-dimensional.");
        }
        if (a.size() != b.size()) {
            throw std::runtime_error("Input arrays must have the same size.");
        }

        int size = a.size();
        std::vector<int> vec_a(a.data(), a.data() + size);
        std::vector<int> vec_b(b.data(), b.data() + size);

        std::vector<int> vec_result = runner_.run(vec_a, vec_b, size);

        py::array_t<int> result_array(vec_result.size());
        std::memcpy(result_array.mutable_data(), vec_result.data(), vec_result.size() * sizeof(int));
        return result_array;
    }

private:
    VAddRunner runner_;
};

PYBIND11_MODULE(libvadd_module_hw, m) { // モジュール名を vadd_module_hw に変更
    m.doc() = "pybind11 wrapper for VAddRunner (Hardware)";

    py::class_<PyVAddRunner>(m, "VAddRunner")
        .def(py::init<const std::string&>())
        .def("run", &PyVAddRunner::run,
             py::arg("a"), py::arg("b"),
             "Runs the vadd kernel with two input numpy arrays and returns the result as a numpy array.");
        // .def("get_kernel_execution_time_ms", &PyVAddRunner::get_kernel_execution_time_ms, // 削除
        //     "Returns the kernel execution time in milliseconds.") // 削除
        // .def("get_total_execution_time_ms", &PyVAddRunner::get_total_execution_time_ms, // 削除
        //     "Returns the total execution time including data transfers in milliseconds."); // 削除
} 
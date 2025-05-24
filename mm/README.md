# MM (Matrix Multiplication) Project

## 概要

このプロジェクトは、16x16の整数行列同士の乗算を行うアクセラレータ (`mm`) のサンプルです。
Vitis HLS を用いてC++でカーネルを記述し、ソフトウェアテストベンチおよびFPGA実機でのテストを行います。
1サイクルで行列乗算を完了するように最適化されています。

## HLSカーネル (`mm.cpp`)

HLSカーネルは `mm` 関数として実装されており、以下のインターフェースを持ちます。

- `a`: 入力行列1 (読み取り専用)
- `b`: 入力行列2 (読み取り専用)
- `c`: 出力行列 (書き込み専用)
- `size`: 行列のサイズ (16固定)

## ビルド手順

ビルドは `Makefile` を使用して行います。

- **ソフトウェアテスト用実行ファイルのビルド**:
  ```bash
  make mm_test_sw
  ```
- **FPGA実機用 `.xclbin` およびホストプログラムのビルド**:
  ```bash
  make mm.xclbin mm_test_hw
  ```
- **Pythonモジュールのビルド**:
  ```bash
  make libmm_module_sw.so libmm_module_hw.so
  ```
- **すべてのビルド**:
  ```bash
  make all
  ```
- **クリーン**:
  ```bash
  make clean
  ```

## 実行手順

### C++ テストベンチ

- **ソフトウェアテスト**:
  ```bash
  make run_test_sw
  ```
- **FPGA実機**:
  ```bash
  make run_test_hw
  ```

### Python テストベンチ

- **ソフトウェアテスト**:
  ```bash
  make run_python_test_sw
  ```
- **FPGA実機**:
  ```bash
  make run_python_test_hw
  ```

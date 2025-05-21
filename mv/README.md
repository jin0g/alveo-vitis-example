# MV (Matrix-Vector Multiplication) Project

## 概要

このプロジェクトは、32x32の整数行列と32要素の整数ベクトルの乗算を行うアクセラレータ (`mv`) のサンプルです。
Vitis HLS を用いてC++でカーネルを記述し、ソフトウェアテストベンチおよびFPGA実機でのテストを行います。
1サイクルで行列ベクトル乗算を完了するように最適化されています。

## HLSカーネル (`mv.cpp`)

HLSカーネルは `mv` 関数として実装されており、以下のインターフェースを持ちます。

- `a`: 入力行列 (読み取り専用)
- `x`: 入力ベクトル (読み取り専用)
- `y`: 出力ベクトル (書き込み専用)
- `size`: 行列/ベクトルのサイズ (32固定)

## ビルド手順

ビルドは `Makefile` を使用して行います。

- **ソフトウェアテスト用実行ファイルのビルド**:
  ```bash
  make mv_test_sw
  ```
- **FPGA実機用 `.xclbin` およびホストプログラムのビルド**:
  ```bash
  make mv.xclbin mv_test_hw
  ```
- **Pythonモジュールのビルド**:
  ```bash
  make libmv_module_sw.so libmv_module_hw.so
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

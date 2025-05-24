# Template Project

## 概要

このプロジェクトは、シンプルなデータコピー操作を行うアクセラレータ (`template`) のテンプレートです。
入力配列から出力配列へのデータコピーを行う基本的な実装を提供します。
Vitis HLS を用いてC++でカーネルを記述し、ソフトウェアテストベンチおよびFPGA実機でのテストを行います。

## HLSカーネル (`template.cpp`)

HLSカーネルは `template` 関数として実装されており、以下のインターフェースを持ちます。

-   `in`: 入力配列 (読み取り専用)
-   `out`: 出力配列 (書き込み専用)
-   `size`: 配列のサイズ

```cpp
extern "C" void template(const int* in, int* out, const int size) {
#pragma HLS INTERFACE m_axi port=in
#pragma HLS INTERFACE m_axi port=out
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE
        out[i] = in[i];
    }
}
```

## ビルド手順

ビルドは `Makefile` を使用して行います。

-   **ソフトウェアテスト用実行ファイルのビルド**:
    ```bash
    make template_test_sw
    ```
-   **FPGA実機用 `.xclbin` およびホストプログラムのビルド**:
    ```bash
    make template.xclbin template_test_hw
    ```
-   **Pythonモジュールのビルド**:
    ```bash
    make libtemplate_module_sw.so libtemplate_module_hw.so
    ```
-   **クリーン**:
    ```bash
    make clean
    ```
-   **完全クリーン (.xclbin と .xo を含む)**:
    ```bash
    make clean_all
    ```

## 実行手順

### C++ テストベンチ

-   **ソフトウェアテスト**:
    ```bash
    make run_test_sw
    ```
-   **FPGA実機**:
    ```bash
    make run_test_hw
    ```

### Python テストベンチ

-   **ソフトウェアテスト**:
    ```bash
    make run_python_test_sw
    ```
-   **FPGA実機**:
    ```bash
    make run_python_test_hw
    ```

## テンプレートとしての使用方法

このプロジェクトは、新しいFPGAアクセラレータを開発する際のテンプレートとして使用できます。
カーネル関数、インターフェース定義、テストベンチの構造などを参考にして、
独自のアクセラレータを実装する際の基盤として活用してください。

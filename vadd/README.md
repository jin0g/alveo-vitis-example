# VADD Project

## 概要

このプロジェクトは、2つの整数ベクトルの要素ごとの加算を行う単純なアクセラレータ (`vadd`) のサンプルです。
Vitis HLS を用いてC++でカーネルを記述し、ソフトウェアテストベンチおよびFPGA実機でのテストを行います。

## HLSカーネル (`vadd.cpp`)

HLSカーネルは `vadd` 関数として実装されており、以下のインターフェースを持ちます。

-   `a`: 入力ベクトル1 (読み取り専用)
-   `b`: 入力ベクトル2 (読み取り専用)
-   `c`: 出力ベクトル (書き込み専用)
-   `size`: ベクトルのサイズ

```cpp
extern "C" void vadd(const int* a, const int* b, int* c, const int size) {
#pragma HLS INTERFACE m_axi port=a
#pragma HLS INTERFACE m_axi port=b
#pragma HLS INTERFACE m_axi port=c
#pragma HLS INTERFACE s_axilite port=size
#pragma HLS INTERFACE s_axilite port=return

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE
        c[i] = a[i] + b[i];
    }
}
```

## ビルド手順

ビルドは `Makefile` を使用して行います。

-   **ソフトウェアエミュレーション用実行ファイルのビルド**:
    ```bash
    make sw_emu
    ```
-   **ハードウェアエミュレーション用実行ファイルのビルド**:
    ```bash
    make hw_emu
    ```
-   **FPGA実機用 `.xclbin` およびホストプログラムのビルド**:
    ```bash
    make build
    ```
-   **Pythonモジュールのビルド**:
    ```bash
    make py
    ```
-   **クリーン**:
    ```bash
    make clean
    ```

## 実行手順

### C++ テストベンチ

-   **ソフトウェアエミュレーション**:
    ```bash
    make run_sw_emu
    ```
-   **ハードウェアエミュレーション**:
    ```bash
    make run_hw_emu
    ```
-   **FPGA実機**:
    ```bash
    make run_hw
    ```

### Python テストベンチ

まずPythonモジュールをビルドします (`make py`)。

-   **ソフトウェアエミュレーション**:
    ```bash
    # (環境変数の設定が必要な場合があります)
    python3 vadd_test.py --xclbin ./vadd.sw_emu.xclbin
    ```
-   **ハードウェアエミュレーション**:
    ```bash
    # (環境変数の設定が必要な場合があります)
    python3 vadd_test.py --xclbin ./vadd.hw_emu.xclbin
    ```
-   **FPGA実機**:
    ```bash
    python3 vadd_test.py --xclbin ./vadd.hw.xclbin
    ```

### C++ ソフトウェアデバッグ

専用のC++ソフトウェアテストベンチを実行します。

```bash
make sw_test
./vadd_sw_test
``` 
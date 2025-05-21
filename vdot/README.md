# VDot (Vector Dot Product) Sample

このサンプルは、2つのベクトル `a` と `b` の内積を計算するカーネル `vdot` を実装しています。
入力ベクトルは `char` (8ビット整数) 型で、出力結果は `int` (32ビット整数) 型です。

## HLSカーネル

- `vdot.cpp`: 内積を計算するHLSカーネルの実装です。
  - `extern "C" void vdot(const char* a, const char* b, int* result, int size)`
    - `a`: 入力ベクトルA (AXI Master)
    - `b`: 入力ベクトルB (AXI Master)
    - `result`: 結果の内積値 (AXI Lite Slave)
    - `size`: ベクトルの要素数 (AXI Lite Slave)

## ビルド

Makefileを使用して各種ターゲットをビルドします。

- `make all`: すべての必要なファイル (xclbin, C++テストベンチ, Pythonモジュール) をビルドします。
- `make $(TOP).xclbin`: HLSカーネルをコンパイル・リンクし、FPGA用のバイナリファイル (`vdot.xclbin`) を生成します。
- `make $(TOP)_test_sw`: C++ソフトウェアテストベンチ (`vdot_test_sw`) をビルドします。
- `make $(TOP)_test_hw`: C++ハードウェアテストベンチ (`vdot_test_hw`) をビルドします。
- `make lib$(TOP)_module_sw.so`: Python用ソフトウェアシミュレーションモジュール (`libvdot_module_sw.so`) をビルドします。
- `make lib$(TOP)_module_hw.so`: Python用ハードウェア実行モジュール (`libvdot_module_hw.so`) をビルドします。

## 実行

- `make run_test_sw`: C++ソフトウェアテストベンチを実行します。
- `make run_test_hw`: C++ハードウェアテストベンチを実行します。FPGAボードが必要です。
  - 例: `make run_test_hw` (内部で `./vdot_test_hw vdot.xclbin` を実行)
- `make run_python_test_sw`: Pythonソフトウェアテストベンチ (`vdot_python_test_sw.py`) を実行します。
- `make run_python_test_hw`: Pythonハードウェアテストベンチ (`vdot_python_test_hw.py`) を実行します。FPGAボードが必要です。

## クリーン

- `make clean`: ビルド生成物 (オブジェクトファイル、実行ファイル、Pythonモジュールなど) を削除します。`.xo` および `.xclbin` ファイルは削除されません。
- `make clean_all`: `.xo` および `.xclbin` を含め、すべてのビルド生成物を削除します。 
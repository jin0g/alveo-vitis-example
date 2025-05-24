#!/usr/bin/env python3
#
#
#
import numpy as np
from libvdot_module_sw import VDotSim # SWモジュールをインポート

def test_vdot_sw():
    DATA_SIZE = 256
    print(f"Running VDOT software test (via Python) with data size: {DATA_SIZE}")

    # テストデータの準備
    # charの範囲 (-128 to 127) でint8として初期化
    a = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)
    b = np.random.randint(-128, 128, size=DATA_SIZE, dtype=np.int8)

    # VDotSimのインスタンス化
    try:
        simulator = VDotSim()
    except Exception as e:
        print(f"Error initializing VDotSim: {e}")
        return

    print("Running VDot software simulation...")
    # 実行
    try:
        result_sim = simulator.run(a, b)
    except Exception as e:
        print(f"Error during VDotSim.run: {e}")
        return

    # 期待値の計算 (Python側, int32で計算してオーバーフローを防ぐ)
    expected_result = np.dot(a.astype(np.int32), b.astype(np.int32))

    # 結果の検証
    if result_sim == expected_result:
        print("Test PASSED!")
        print(f"Simulated result: {result_sim}")
        print(f"Expected result: {expected_result}")
    else:
        print("Test FAILED!")
        print(f"Simulated result: {result_sim}")
        print(f"Expected result: {expected_result}")
        print(f"Difference: {result_sim - expected_result}")

if __name__ == "__main__":
    test_vdot_sw()  
import numpy as np
from libvadd_module_sw import VAddSim # SWモジュールをインポート

def test_vadd_sw():
    DATA_SIZE = 256
    print(f"Running VADD software test (via Python) with data size: {DATA_SIZE}")

    # テストデータの準備
    a = np.random.randint(0, 100, size=DATA_SIZE, dtype=np.int32)
    b = np.random.randint(0, 100, size=DATA_SIZE, dtype=np.int32)

    # VAddSimのインスタンス化
    try:
        simulator = VAddSim()
    except Exception as e:
        print(f"Error initializing VAddSim: {e}")
        return

    print("Running VAdd software simulation...")
    # 実行
    try:
        result_sim = simulator.run(a, b)
    except Exception as e:
        print(f"Error during VAddSim.run: {e}")
        return

    # 期待値の計算 (Python側)
    expected_result = a + b

    # 結果の検証
    if np.array_equal(result_sim, expected_result):
        print("Test PASSED!")
    else:
        print("Test FAILED!")
        for i in range(DATA_SIZE):
            if result_sim[i] != expected_result[i]:
                print(f"Mismatch at index {i}: SIM={result_sim[i]}, Expected={expected_result[i]}")
                # Show only the first few mismatches for brevity
                if i > 5:
                    print("...")
                    break

if __name__ == "__main__":
    test_vadd_sw() 
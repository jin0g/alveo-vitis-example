#!/bin/bash


source /opt/xilinx/xrt/setup.sh

RESULTS_FILE="test_results.txt"
RESULTS_MD="results_table.md"

BIT_WIDTHS=(32 64 128 256 512 1024)
BURST_LENGTHS=(64 128 256)
NUM_ITERATIONS=5

echo "# バースト転送最適化テスト結果" > $RESULTS_FILE
echo "実行日時: $(date)" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

echo "## バースト転送最適化テスト結果" > $RESULTS_MD
echo "" >> $RESULTS_MD
echo "| ビット幅 | バースト長 | スループット (M Ops/sec) | 帯域幅 (MB/s) | 理論値比 (%) |" >> $RESULTS_MD
echo "|---------|-----------|------------------------|--------------|------------|" >> $RESULTS_MD

for bit_width in 32 64; do
    for burst_length in "${BURST_LENGTHS[@]}"; do
        xclbin_file="burst_test_${bit_width}.xclbin"
        
        if [ -f "$xclbin_file" ]; then
            echo "=== テスト実行: ${bit_width}ビット, バースト長 ${burst_length} ===" >> $RESULTS_FILE
            echo "簡易テストハーネスを使用" >> $RESULTS_FILE
            
            ./burst_test_simple_hw $xclbin_file $bit_width $burst_length | tee -a $RESULTS_FILE
            
            throughput=$(grep "Throughput:" $RESULTS_FILE | tail -1 | awk '{print $2}')
            bandwidth=$(grep "Bandwidth:" $RESULTS_FILE | tail -1 | awk '{print $2}')
            
            if [ ! -z "$bandwidth" ]; then
                ratio=$(echo "scale=2; $bandwidth / 16384 * 100" | bc)
                echo "PCIe 3.0 x16理論値比: ${ratio}%" >> $RESULTS_FILE
            else
                ratio="N/A"
            fi
            
            echo "| $bit_width | $burst_length | $throughput | $bandwidth | $ratio |" >> $RESULTS_MD
        else
            echo "警告: ${xclbin_file}が見つかりません。${bit_width}ビットのテストをスキップします。" >> $RESULTS_FILE
        fi
    done
done

for bit_width in "${BIT_WIDTHS[@]}"; do
    if [ "$bit_width" -eq 32 ] || [ "$bit_width" -eq 64 ]; then
        continue
    fi
    
    for burst_length in "${BURST_LENGTHS[@]}"; do
        xclbin_file="burst_test_${bit_width}.xclbin"
        
        if [ -f "$xclbin_file" ]; then
            echo "=== テスト実行: ${bit_width}ビット, バースト長 ${burst_length} ===" >> $RESULTS_FILE
            echo "標準テストハーネスを使用" >> $RESULTS_FILE
            
            ./burst_test_test_hw $xclbin_file $bit_width $burst_length | tee -a $RESULTS_FILE
            
            throughput=$(grep "Throughput:" $RESULTS_FILE | tail -1 | awk '{print $2}')
            bandwidth=$(grep "Bandwidth:" $RESULTS_FILE | tail -1 | awk '{print $2}')
            
            if [ ! -z "$bandwidth" ]; then
                ratio=$(echo "scale=2; $bandwidth / 16384 * 100" | bc)
                echo "PCIe 3.0 x16理論値比: ${ratio}%" >> $RESULTS_FILE
            else
                ratio="N/A"
            fi
            
            echo "| $bit_width | $burst_length | $throughput | $bandwidth | $ratio |" >> $RESULTS_MD
        else
            echo "警告: ${xclbin_file}が見つかりません。${bit_width}ビットのテストをスキップします。" >> $RESULTS_FILE
        fi
    done
done

echo "" >> $RESULTS_FILE
echo "テスト完了: $(date)" >> $RESULTS_FILE

echo "テスト結果は ${RESULTS_FILE} と ${RESULTS_MD} に保存されました。"

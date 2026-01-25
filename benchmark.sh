#!/bin/bash

echo "=== M2 MacBook Air Performance Analysis ==="
echo "4 performance + 4 efficiency cores = 8 threads optimal"
echo ""

# Test thread scaling on M2
for threads in 1 2 4 8; do
    echo "Testing with $threads threads:"
    OMP_NUM_THREADS=$threads ./pricing_optimized.out data/synthetic/european-options/options_small.csv 2>/dev/null | grep "Time:" | head -1
done

echo ""
echo "=== Memory Usage Analysis ==="
/usr/bin/time -l ./pricing_optimized.out data/synthetic/european-options/options_medium.csv 2>&1 | grep -E "(real|maximum resident set size)"

echo ""
echo "=== Large Dataset Performance ==="
./pricing_optimized.out data/synthetic/european-options/options_large.csv

#!/bin/bash

DATASET=$1
CSV_FILE="data/synthetic/european-options/options_${DATASET}.csv"

if [ ! -f "$CSV_FILE" ]; then
    echo "Error: Dataset file not found: $CSV_FILE"
    exit 1
fi

NUM_OPTIONS=$(tail -n +2 "$CSV_FILE" | wc -l | tr -d ' ')

echo "=== Running: $DATASET dataset ($NUM_OPTIONS options) ==="
echo ""

echo "--- Baseline ---"
BASELINE_OUTPUT=$(./bin/pricing.out "$CSV_FILE" 2>&1)
echo "$BASELINE_OUTPUT"
BASELINE_TIME=$(echo "$BASELINE_OUTPUT" | grep "Total time:" | awk '{print $3}')
BASELINE_THROUGHPUT=$(echo "$BASELINE_OUTPUT" | grep "Throughput:" | awk '{print $2}')

echo ""
echo "--- Optimized ---"
OPTIMIZED_OUTPUT=$(./bin/pricing.out --optimized "$CSV_FILE" 2>&1)
echo "$OPTIMIZED_OUTPUT"
OPTIMIZED_TIME=$(echo "$OPTIMIZED_OUTPUT" | grep "Total time:" | awk '{print $3}')
OPTIMIZED_THROUGHPUT=$(echo "$OPTIMIZED_OUTPUT" | grep "Throughput:" | awk '{print $2}')

# Output results in parseable format for aggregation
echo "RESULT|$DATASET|$NUM_OPTIONS|$BASELINE_TIME|$BASELINE_THROUGHPUT|$OPTIMIZED_TIME|$OPTIMIZED_THROUGHPUT"

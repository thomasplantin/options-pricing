CXX = g++
CXXFLAGS = -std=c++20 -O3 -Isrc -march=native -pthread -Wall -Wextra -ffast-math

SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin
TARGET_TEST_BIN = $(BIN_DIR)/$(TEST_DIR)

TARGET = $(BIN_DIR)/pricing.out

SOURCES = $(SRC_DIR)/main.cpp

HEADERS = $(SRC_DIR)/core/option.hpp \
          $(SRC_DIR)/core/constants.hpp \
          $(SRC_DIR)/math/normal.hpp \
          $(SRC_DIR)/math/black_scholes.hpp \
          $(SRC_DIR)/monte_carlo/baseline.hpp \
          $(SRC_DIR)/monte_carlo/optimized.hpp \
          $(SRC_DIR)/utils/csv_loader.hpp

TEST_SOURCES = $(wildcard $(TEST_DIR)/**/*_test.cpp)
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.cpp,$(TARGET_TEST_BIN)/%.out,$(TEST_SOURCES))

.PHONY: all clean benchmark test

all: $(TARGET)

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(TARGET_TEST_BIN): $(BIN_DIR)
	mkdir $(TARGET_TEST_BIN)

$(TARGET): $(SOURCES) $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

$(TARGET_TEST_BIN)/%.out: $(TEST_DIR)/%.cpp $(HEADERS) | $(TARGET_TEST_BIN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I/opt/homebrew/opt/googletest/include $(TEST_DIR)/$*.cpp -o $@ -L/opt/homebrew/opt/googletest/lib -lgtest -lgtest_main -pthread

benchmark: $(TARGET)
	@echo "=== Running Benchmarks ==="
	@echo ""
	@echo "Running small dataset..."
	@SMALL=$$(./benchmark_runner.sh small 2>&1 | grep "^RESULT"); \
	echo "✓ Small dataset complete"; \
	echo ""; \
	echo "Running medium dataset..."; \
	MEDIUM=$$(./benchmark_runner.sh medium 2>&1 | grep "^RESULT"); \
	echo "✓ Medium dataset complete"; \
	echo ""; \
	echo "Running large dataset..."; \
	LARGE=$$(./benchmark_runner.sh large 2>&1 | grep "^RESULT"); \
	echo "✓ Large dataset complete"; \
	echo ""; \
	echo "=== Benchmark Summary ==="; \
	echo ""; \
	printf "%-10s | %8s | %-37s | %-37s | %s\n" "Dataset" "Options" "Baseline" "Optimized" "Speedup"; \
	printf "%-10s | %8s | %-14s %-15s | %-14s %-15s | %s\n" "" "" "Time (ms)" "Throughput (M paths/s)" "Time (ms)" "Throughput (M paths/s)" ""; \
	echo "-----------|----------|---------------------------------------|---------------------------------------|----------"; \
	echo "$$SMALL\n$$MEDIUM\n$$LARGE" | while IFS='|' read -r prefix dataset opts btime btput otime otput; do \
		if [ "$$prefix" = "RESULT" ]; then \
			speedup=$$(echo "scale=2; $$btime / $$otime" | bc 2>/dev/null || echo "N/A"); \
			printf "%-10s | %8s | %14s %22s | %14s %22s | %-8s\n" "$$dataset" "$$opts" "$$btime" "$$btput" "$$otime" "$$otput" "$${speedup}x"; \
		fi \
	done

benchmark-run: $(TARGET)
	@if [ -z "$(data)" ]; then \
		echo "Error: data parameter required (small, medium, or large)"; \
		exit 1; \
	fi
	@./benchmark_runner.sh $(data) 2>&1 | grep -v "^RESULT"

test: $(TEST_TARGETS)
	@for test in $(TEST_TARGETS); do \
		echo "=== Running $$test ==="; \
		./$$test; \
		echo ""; \
	done

clean:
	rm -rf ./$(BIN_DIR)

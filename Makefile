CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -pthread -Wall -Wextra -ffast-math
TARGET = pricing.out
TARGET_OPT = pricing_optimized.out
SRC_DIR = src
TEST_DIR = tests

SOURCES = $(SRC_DIR)/main.cpp
SOURCES_OPT = $(SRC_DIR)/main_optimized.cpp
HEADERS = $(SRC_DIR)/option.hpp \
          $(SRC_DIR)/constants.hpp \
          $(SRC_DIR)/normal.hpp \
          $(SRC_DIR)/black_scholes.hpp \
          $(SRC_DIR)/monte_carlo.hpp \
          $(SRC_DIR)/monte_carlo_optimized.hpp \
          $(SRC_DIR)/csv_loader.hpp

TEST_SOURCES = $(TEST_DIR)/normal_test.cpp \
               $(TEST_DIR)/black_scholes_test.cpp \
               $(TEST_DIR)/monte_carlo_test.cpp
TEST_TARGETS = $(TEST_DIR)/normal_test.out \
               $(TEST_DIR)/black_scholes_test.out \
               $(TEST_DIR)/monte_carlo_test.out

.PHONY: all clean benchmark test

all: $(TARGET) $(TARGET_OPT)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

$(TARGET_OPT): $(SOURCES_OPT) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES_OPT) -o $(TARGET_OPT)

benchmark: $(TARGET) $(TARGET_OPT)
	@echo "=== Baseline Implementation ==="
	@./$(TARGET) data/synthetic/european-options/options_medium.csv
	@echo ""
	@echo "=== Optimized Implementation ==="
	@./$(TARGET_OPT) data/synthetic/european-options/options_medium.csv

$(TEST_DIR)/normal_test.out: $(TEST_DIR)/normal_test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I/opt/homebrew/opt/googletest/include $(TEST_DIR)/normal_test.cpp -o $@ -L/opt/homebrew/opt/googletest/lib -lgtest -lgtest_main -pthread

$(TEST_DIR)/black_scholes_test.out: $(TEST_DIR)/black_scholes_test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I/opt/homebrew/opt/googletest/include $(TEST_DIR)/black_scholes_test.cpp -o $@ -L/opt/homebrew/opt/googletest/lib -lgtest -lgtest_main -pthread

$(TEST_DIR)/monte_carlo_test.out: $(TEST_DIR)/monte_carlo_test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I/opt/homebrew/opt/googletest/include $(TEST_DIR)/monte_carlo_test.cpp -o $@ -L/opt/homebrew/opt/googletest/lib -lgtest -lgtest_main -pthread

test: $(TEST_TARGETS)
	@echo "=== Running Normal Distribution Tests ==="
	@./$(TEST_DIR)/normal_test.out
	@echo ""
	@echo "=== Running Black-Scholes Tests ==="
	@./$(TEST_DIR)/black_scholes_test.out
	@echo ""
	@echo "=== Running Monte Carlo Tests ==="
	@./$(TEST_DIR)/monte_carlo_test.out

clean:
	rm -f $(TARGET) $(TARGET_OPT) $(TEST_TARGETS)

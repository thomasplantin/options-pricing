CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -pthread -Wall -Wextra
TARGET = pricing.out
SRC_DIR = src
BUILD_DIR = build

SOURCES = $(SRC_DIR)/main.cpp
HEADERS = $(SRC_DIR)/option.hpp \
          $(SRC_DIR)/constants.hpp \
          $(SRC_DIR)/normal.hpp \
          $(SRC_DIR)/black_scholes.hpp \
          $(SRC_DIR)/monte_carlo.hpp \
          $(SRC_DIR)/csv_loader.hpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

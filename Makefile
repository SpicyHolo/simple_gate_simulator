# C++ Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -g -I./include

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

# Google Test directories (adjust these paths based on your installation)
GTEST_INCLUDE = /usr/include/gtest
GTEST_LIB = /usr/lib

# Output directories
TARGET = $(BIN_DIR)/main
TEST_TARGET = $(BIN_DIR)/test

# Source and Object files
SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/gate_logic.cpp
TEST_SRC = $(TEST_DIR)/test.cpp $(SRC_DIR)/gate_logic.cpp
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/gate_logic.o
TEST_OBJ = $(BUILD_DIR)/test.o $(BUILD_DIR)/gate_logic.o

# Linker flags for Google Test
LDFLAGS = -L$(GTEST_LIB) -lgtest -lgtest_main

# Default target
all: $(TARGET) $(TEST_TARGET)

# Build main application
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Build tests
$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) $(LDFLAGS)

# Object file rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

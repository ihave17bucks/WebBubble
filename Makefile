# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Iinclude
CXXFLAGS = -Wall -Wextra -Iinclude -std=c++17
LDFLAGS = -lstdc++ 

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TEST_DIR = tests

# Source files
COMMON_SOURCES = $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/ast.c $(SRC_DIR)/interpreter.c
COMMON_OBJECTS = $(BUILD_DIR)/lexer.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/interpreter.o

# C++ modules (for advanced features)
CPP_SOURCES = $(SRC_DIR)/json.cpp $(SRC_DIR)/string_utils.cpp
CPP_OBJECTS = $(BUILD_DIR)/json.o $(BUILD_DIR)/string_utils.o

# Executables
TARGET_REPL = $(BUILD_DIR)/webbubble
TARGET_SERVER = $(BUILD_DIR)/webbubble-server
TARGET_DEMO = $(BUILD_DIR)/webbubble-demo

# Default target - build all
all: $(TARGET_REPL) $(TARGET_SERVER) $(TARGET_DEMO)

# Build the REPL/test executable
$(TARGET_REPL): $(COMMON_OBJECTS) $(CPP_OBJECTS) $(BUILD_DIR)/main.o
	$(CXX) $(COMMON_OBJECTS) $(CPP_OBJECTS) $(BUILD_DIR)/main.o -o $(TARGET_REPL) $(LDFLAGS)
	@echo "REPL build complete! Run with: ./$(TARGET_REPL)"

# Build the HTTP server executable
$(TARGET_SERVER): $(COMMON_OBJECTS) $(CPP_OBJECTS) $(BUILD_DIR)/http_server.o $(BUILD_DIR)/server.o
	$(CXX) $(COMMON_OBJECTS) $(CPP_OBJECTS) $(BUILD_DIR)/http_server.o $(BUILD_DIR)/server.o -o $(TARGET_SERVER) $(LDFLAGS)
	@echo "Server build complete! Run with: ./$(TARGET_SERVER)"

# Build the hybrid demo executable
$(TARGET_DEMO): $(CPP_OBJECTS) $(BUILD_DIR)/demo_hybrid.o
	$(CXX) $(CPP_OBJECTS) $(BUILD_DIR)/demo_hybrid.o -o $(TARGET_DEMO) $(LDFLAGS)
	@echo "Demo build complete! Run with: ./$(TARGET_DEMO)"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET_REPL) $(TARGET_SERVER) $(TARGET_DEMO)
	@echo "Cleaned build directory"

# Clean everything including build directory
cleanall:
	rm -rf $(BUILD_DIR)
	@echo "Removed build directory"

# Run the REPL
run-repl: $(TARGET_REPL)
	./$(TARGET_REPL)

# Run the server
run-server: $(TARGET_SERVER)
	./$(TARGET_SERVER)

# Run the hybrid demo
run-demo: $(TARGET_DEMO)
	./$(TARGET_DEMO)

# Default run target (server)
run: run-server

# Show help
help:
	@echo "Available targets:"
	@echo "  make              - Build both REPL and server"
	@echo "  make clean        - Remove object files and executables"
	@echo "  make cleanall     - Remove entire build directory"
	@echo "  make run          - Build and run the HTTP server (default port 8080)"
	@echo "  make run-server   - Build and run the HTTP server"
	@echo "  make run-repl     - Build and run the REPL/test program"
	@echo "  make help         - Show this help message"

.PHONY: all clean cleanall run run-server run-repl help

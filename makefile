# Compiler and Flags
CC          := gcc
CFLAGS      := -Wpedantic -Wall -Wextra -Werror -g -Iinclude
LDFLAGS     := -lm

# Directories
SRC_DIR     := src
BUILD_DIR   := build
BIN_DIR     := bin

# Target executable name
TARGET      := $(BIN_DIR)/roll

# Find all source files in SRC_DIR
SRCS        := $(wildcard $(SRC_DIR)/*.c)
# Convert source file list to object file list in BUILD_DIR
OBJS        := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default rule: build the executable
all: prepare $(TARGET)

# Create necessary directories
prepare:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile each .c file into a .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all prepare clean

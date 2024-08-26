
BUILD_DIR = build
BUILD_OBJ = src.out
BUILD_PATH = $(BUILD_DIR)/$(BUILD_OBJ)

SRC_DIRS = ./

GCC = gcc
MAIN_FLAGS = -std=c99 -g -O0 -o $(BUILD_PATH)
WARNINGS_FLAGS = -Wall -Wextra -Wpedantic -Wduplicated-branches -Wduplicated-cond -Wcast-qual -Wconversion -Wsign-conversion -Wlogical-op -Werror
SANITIZER_FLAGS = -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope
FLAGS = $(MAIN_FLAGS) $(WARNINGS_FLAGS) $(SANITIZER_FLAGS)

# Sources and headers
SOURCES = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
SOURCES = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)

all:
	$(GCC) $(FLAGS) $(SOURCES)

.PHONY: all

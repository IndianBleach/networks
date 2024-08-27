SRC_DIR = ./src
UTILS_SRC_DIR = $(SRC_DIR)/utils


BUILD_DIR = $(SRC_DIR)/build
BUILD_OBJ = src.out
BUILD_PATH = $(BUILD_DIR)/$(BUILD_OBJ)

GCC = gcc
MAIN_FLAGS = -std=c99 -g -O0
# WARNINGS_FLAGS = -Wall -Wextra -Wpedantic -Wduplicated-branches -Wduplicated-cond -Wcast-qual -Wconversion -Wsign-conversion -Wlogical-op -Werror
WARNINGS_FLAGS = -Wall -Wextra -Wpedantic -Wduplicated-branches -Wduplicated-cond -Wcast-qual -Wconversion -Wsign-conversion -Wlogical-op
SANITIZER_FLAGS = -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined -fsanitize-address-use-after-scope
FLAGS = $(MAIN_FLAGS) $(WARNINGS_FLAGS) $(SANITIZER_FLAGS)

# Sources and headers
SOURCES = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
SOURCES = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)

DEPS = $(BUILD_DIR)/vector.o

vector:
	$(GCC) $(FLAGS) -c $(UTILS_SRC_DIR)/vector.c -o $(BUILD_DIR)/vector.o

all: vector
	$(GCC) $(FLAGS) $(SOURCES)

scanner: vector
	$(GCC) $(FLAGS) $(DEPS) $(SRC_DIR)/http/scanner.c -o $(BUILD_DIR)/scanner

parser: vector
	$(GCC) $(FLAGS) $(DEPS) $(SRC_DIR)/http/parser.c -o $(BUILD_DIR)/parser

clean:
	# cd $(BUILD_DIR)
	rm $(BUILD_DIR)/*.out $(BUILD_DIR)/*.o $(BUILD_DIR)/scanner $(BUILD_DIR)/server $(BUILD_DIR)/parser

.PHONY: all

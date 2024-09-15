SRC_DIR = ./src
UTILS_SRC_DIR = $(SRC_DIR)/utils


BUILD_DIR = $(SRC_DIR)/build
BUILD_OBJ = src.out
BUILD_PATH = $(BUILD_DIR)/$(BUILD_OBJ)

INCLUDE_FLAGS=-I ./src/include
SSL_FLAGS = -lssl -lcrypto

GCC = gcc
MAIN_FLAGS = -std=c99 -g -O0 $(INCLUDE_FLAGS) $(SSL_FLAGS)

# WARNINGS_FLAGS = -Wall -Wextra -Wpedantic -Wduplicated-branches -Wduplicated-cond -Wcast-qual -Wconversion -Wsign-conversion -Wlogical-op -Werror
WARNINGS_FLAGS = -Wall -Wextra -Wpedantic -Wduplicated-branches -Wduplicated-cond -Wcast-qual -Wconversion -Wsign-conversion -Wlogical-op
SANITIZER_FLAGS = -fsanitize=address #-fsanitize=pointer-compare -fsanitize=pointer-subtract # -fsanitize=undefined -fsanitize-address-use-after-scope #-fsanitize=leak
FLAGS = $(MAIN_FLAGS) $(WARNINGS_FLAGS)  $(SANITIZER_FLAGS)
#
# Sources and headers
SOURCES = $(shell find $(SRC_DIR) $(UTILS_SRC_DIR) -name *.cpp -or -name *.c)
HEADERS = $(shell find $(SRC_DIR) $(UTILS_SRC_DIR) -name *.hpp -or -name *.h)

DEPS = $(BUILD_DIR)/vector.o

TEST_DIR_HEADERS = ./src/
TEST_DIR_SRC = ./src/
TEST_HEADERS = $(shell find $(TEST_DIR_HEADERS) -name *.h)
TEST_SRC = $(shell find $(TEST_DIR_SRC) -name *.c)

core.core2:
	$(GCC) $(FLAGS) ./src/core/core.c -o $(BUILD_OBJ)

server.test:
	$(GCC) $(FLAGS) $(TEST_SRC) $(TEST_HEADERS) -o $(BUILD_OBJ)

server:
	$(GCC) $(FLAGS) $(SRC_DIR)/http/server/server.c -o $(BUILD_OBJ)

test: test
	$(GCC) $(FLAGS) -c $(SRC_DIR)/http/server/new.c -o $(BUILD_OBJ)

vector:
	$(GCC) $(FLAGS) -c $(UTILS_SRC_DIR)/vector.c -o $(BUILD_DIR)/vector.o

all: vector
	$(GCC) $(FLAGS) $(SOURCES)

scanner: vector
	$(GCC) $(FLAGS) $(DEPS) $(SRC_DIR)/http/token.c $(SRC_DIR)/http/scanner.c -o $(BUILD_DIR)/scanner

parser: vector
	$(GCC) $(FLAGS) $(DEPS) $(SRC_DIR)/http/token.c $(SRC_DIR)/http/parser.c $(SRC_DIR)/http/scanner.c -o $(BUILD_DIR)/parser

clean:
	# cd $(BUILD_DIR)
	rm $(BUILD_DIR)/*.out $(BUILD_DIR)/*.o $(BUILD_DIR)/scanner $(BUILD_DIR)/server $(BUILD_DIR)/parser

.PHONY: test3

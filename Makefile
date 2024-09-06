CC=clang
LIB_DIR=lib
SRC_DIR=src
BUILD_DIR=build

CFLAGS=-Wall -Wextra -std=c11 -pedantic -g
# LDFLAGS=-I./lib/c_utils/include/ -L./lib/c_utils/build/
LDFLAGS=-I./lib/c_utils/include/
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c -o $@

build/libhttpserver.a: $(OBJ_FILES)
	ar -rcs $@ $^

fmt:
	clang-format **/*.c **/*.h -i

clean:
	rm -f $(BUILD_DIR)/*

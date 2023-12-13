CC=cc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -g
SRC_DIR=src
BUILD_DIR=build
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/libserver.a: $(OBJ_FILES)
	ar -rcs $@ $^

clean:
	rm -f $(BUILD_DIR)/*.o build/libserver.a
CC := i686-w64-mingw32-gcc
CC_FLAGS := -Wall -Wextra -Wpedantic -m32 -shared -I include/ -L lib
BIN := remote.dll
LIB_FLAGS := -lthcrap_tsa -lthcrap -ljansson -lws2_32
SRC_DIR := src
OBJ_DIR := obj

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

.PHONY: all clean

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJ_FILES)

$(BIN): $(OBJ_FILES)
	$(CC) $(CC_FLAGS) $^ -o $@ $(LIB_FLAGS)

$(OBJ_FILES): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CC_FLAGS) -c $< -o $@

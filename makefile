CC = gcc -g3
CFLAGS = -Wall -Wextra -Iinc
SRC_DIR = src
INC_DIR = inc
OUT_DIR = output
TARGET = $(OUT_DIR)/program

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/utils.c $(SRC_DIR)/dns_module.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SOURCES))
HEADERS = $(wildcard $(INC_DIR)/*.h)


all: $(TARGET)

$(TARGET): $(OBJECTS) | $(OUT_DIR)
	$(CC) $(OBJECTS) -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



clean:
	rm -rf $(OUT_DIR)/*.o $(TARGET)

.PHONY: all clean
CC = gcc
CFLAGS = -Wall -Iinc
SRC_DIR = src
INC_DIR = inc
OUT_DIR = output
TARGET = $(OUT_DIR)/program

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/utils.c
OBJECTS = $(OUT_DIR)/main.o $(OUT_DIR)/utils.o
HEADERS = $(INC_DIR)/utils.h

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(OUT_DIR)
	$(CC) $(OBJECTS) -o $@

$(OUT_DIR)/main.o: $(SRC_DIR)/main.c $(HEADERS) | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/utils.o: $(SRC_DIR)/utils.c $(HEADERS) | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



clean:
	rm -rf $(OUT_DIR)/*.o $(TARGET)

.PHONY: all clean
CC = gcc 
CFLAGS = -Wall -Wextra -I$(INC_DIR) -g3
SRC_DIR = src
INC_DIR = ../inc
OUT_DIR = out
TARGET = $(OUT_DIR)/server

SOURCES = $(SRC_DIR)/server.c 
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
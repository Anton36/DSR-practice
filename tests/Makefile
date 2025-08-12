
CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -O0 -g3 -I../inc -I../src

LIBS = -lcheck_pic -lrt -lm -lsubunit 

SRC_DIR = ../src
INC_DIR = ../inc
TEST_DIR = .
OUT_DIR = ../output
TEST_OUT = out


$(OUT_DIR) $(TEST_OUT):
	mkdir -p $@

SOURCES = $(SRC_DIR)/utils.c $(SRC_DIR)/dns_module.c $(SRC_DIR)/linked_list.c $(SRC_DIR)/dns_perf.c

OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(TEST_OUT)/%.o,$(SOURCES))

TEST_SOURCES = $(TEST_DIR)/test_dns_module.c $(TEST_DIR)/runner.c $(TEST_DIR)/test_linked_list.c $(TEST_DIR)/test_main_parsing.c $(TEST_DIR)/test_utils.c

TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c,$(TEST_OUT)/%.o,$(TEST_SOURCES))

TEST_BIN = $(TEST_OUT)/runner
COVERAGE_DIR = $(TEST_OUT)/coverage
COVERAGE_INFO = $(TEST_OUT)/coverage.info


.PHONY: all
all: $(TEST_BIN)


$(TEST_BIN): $(TEST_OBJECTS) $(OBJECTS) | $(TEST_OUT) 
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -o $@ $^ $(LDFLAGS) $(LIBS)


$(TEST_OUT)/%.o: $(SRC_DIR)/%.c | $(TEST_OUT)
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -c -o $@ $<


$(TEST_OUT)/%.o: $(TEST_DIR)/%.c | $(TEST_OUT)
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -c -o $@ $<


.PHONY: run
run: $(TEST_BIN)
	#
	(cd $(TEST_OUT) && ./runner)


.PHONY: coverage
coverage: clean all run
	cd $(TEST_OUT) && \
	lcov --capture --directory . --base-directory .. --output-file coverage.info
	
	cd $(TEST_OUT) && \
	genhtml coverage.info --output-directory coverage/ --prefix $(abspath ..)

.PHONY: clean
clean:
	rm -rf $(TEST_OUT)

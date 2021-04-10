TARGET := btb
SRC_DIR := src
TARGET_TEST := btb-test
TEST_DIR := test

CC := gcc
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
CSTD := c17
CFLAGS := -O3 -std=$(CSTD) $(WARNINGS)

.PHONY: clean test

all: test $(TARGET)

$(TARGET): bencode.o $(SRC_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET) $?

test: bencode.o bencode_test.o $(TEST_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET_TEST) $?
	@ ./$(TARGET_TEST)

bencode_test.o: $(TEST_DIR)/bencode_test.c
	@ $(CC) $(CFLAGS) -c $?

bencode.o: $(SRC_DIR)/bencode.c
	@ $(CC) $(CFLAGS) -c $?

clean:
	@ $(RM) $(TARGET) $(TARGET_TEST) *.o

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

$(TARGET): btb.o $(SRC_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET) $?

test: btb.o $(TEST_DIR)/btb_test.c
	@ $(CC) $(CFLAGS) -o $(TARGET_TEST) $?
	@ ./$(TARGET_TEST)

btb.o: $(SRC_DIR)/btb.c
	@ $(CC) $(CFLAGS) -c $?

clean:
	@ $(RM) btb btb-test *.o

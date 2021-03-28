TARGET := btb
SRC_DIR := src
TEST_DIR := test

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
CSTD := c17
CFLAGS := -O3 -std=$(CSTD) $(WARNINGS)

.PHONY: clean test

all: btb

btb: btb.o $(SRC_DIR)/main.c
	@ gcc $(CFLAGS) -o $(TARGET) $?

btb.o: $(SRC_DIR)/btb.c
	@ gcc $(CFLAGS) -c $?

clean:
	@ $(RM) btb btb-test *.o

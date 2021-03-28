TARGET := btb
SRC_DIR := src
TEST_DIR := test

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
CSTD := c17
CFLAGS := -O3 -std=$(CSTD) $(WARNINGS)

.PHONY: clean

all: btb

btb: btb.o
	gcc $(CFLAGS) -o $(TARGET) $(SRC_DIR)/main.c btb.o

btb.o: $(SRC_DIR)/btb.c
	gcc $(CFLAGS) -c $(SRC_DIR)/btb.c

clean:
	$(RM) btb btb-test *.o

TARGET := btb
SRC_DIR := src
TARGET_TEST := btb-test
TEST_DIR := test

CC := gcc
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wstrict-prototypes
CSTD := c17
CFLAGS := -O3 -std=$(CSTD) $(WARNINGS)
LIBS := -lcrypto -lcurl

.PHONY: clean test

all: test $(TARGET)

$(TARGET): bencode.o $(SRC_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET) $? $(LIBS)

test: bencode.o bencode_test.o torrent_file.o torrent_file_test.o protocol.o protocol_test.o $(TEST_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET_TEST) $? $(LIBS)
	@ ./$(TARGET_TEST)

protocol_test.o: $(TEST_DIR)/protocol_test.c
	@ $(CC) $(CFLAGS) -c $?

protocol.o: $(SRC_DIR)/protocol.c
	@ $(CC) $(CFLAGS) -c $?

torrent_file_test.o: $(TEST_DIR)/torrent_file_test.c
	@ $(CC) $(CFLAGS) -c $?

torrent_file.o: $(SRC_DIR)/torrent_file.c
	@ $(CC) $(CFLAGS) -c $?

bencode_test.o: $(TEST_DIR)/bencode_test.c
	@ $(CC) $(CFLAGS) -c $?

bencode.o: $(SRC_DIR)/bencode.c
	@ $(CC) $(CFLAGS) -c $?

clean:
	@ $(RM) $(TARGET) $(TARGET_TEST) *.o

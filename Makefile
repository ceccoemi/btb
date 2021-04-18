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

$(TARGET): tokenizer.o $(SRC_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET) $? $(LIBS)

test: tokenizer.o tokenizer_test.o file_buf.o file_buf_test.o torrent_file.o torrent_file_test.o peer.o peer_test.o protocol.o protocol_test.o $(TEST_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET_TEST) $? $(LIBS)
	@ ./$(TARGET_TEST)

protocol_test.o: $(TEST_DIR)/protocol_test.c
	@ $(CC) $(CFLAGS) -c $?

protocol.o: $(SRC_DIR)/protocol.c
	@ $(CC) $(CFLAGS) -c $?

peer_test.o: $(TEST_DIR)/peer_test.c
	@ $(CC) $(CFLAGS) -c $?

peer.o: $(SRC_DIR)/peer.c
	@ $(CC) $(CFLAGS) -c $?

torrent_file_test.o: $(TEST_DIR)/torrent_file_test.c
	@ $(CC) $(CFLAGS) -c $?

torrent_file.o: $(SRC_DIR)/torrent_file.c
	@ $(CC) $(CFLAGS) -c $?

file_buf_test.o: $(TEST_DIR)/file_buf_test.c
	@ $(CC) $(CFLAGS) -c $?

file_buf.o: $(SRC_DIR)/file_buf.c
	@ $(CC) $(CFLAGS) -c $?

tokenizer_test.o: $(TEST_DIR)/tokenizer_test.c
	@ $(CC) $(CFLAGS) -c $?

tokenizer.o: $(SRC_DIR)/tokenizer.c
	@ $(CC) $(CFLAGS) -c $?

clean:
	@ $(RM) $(TARGET) $(TARGET_TEST) *.o

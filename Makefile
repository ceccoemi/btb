TARGET := btb
SRC_DIR := src
TARGET_TEST := btb-test
TEST_DIR := test

CC := gcc
WARNINGS := -Wall -Werror
CSTD := c17
CFLAGS_ALL := -std=$(CSTD) $(WARNINGS)
CFLAGS_TEST := -g
CFLAGS_RELEASE := -O3
LIBS := -lcrypto -lcurl -lpthread -lm
VALGRIND_SUPP_FILE := valgrind.supp
VALGRIND_CMD := valgrind --leak-check=full --show-leak-kinds=all --suppressions=$(VALGRIND_SUPP_FILE)

.PHONY: clean test

all: test build

build: src_obj $(SRC_DIR)/main.c
	$(CC) $(CFLAGS_ALL) $(CFLAGS_RELEASE) -o $(TARGET) $? $(LIBS)

test: src_obj test_obj $(TEST_DIR)/main.c
	$(CC) $(CFLAGS_ALL) $(CFLAGS_TEST) -o $(TARGET_TEST) $? $(LIBS)
	$(VALGRIND_CMD) ./$(TARGET_TEST)

test_obj: \
	tokenizer_test.o \
	file_buf_test.o \
	torrent_file_test.o \
	peer_test.o \
	handshake_msg_test.o \
	message_test.o \
	bitfield_test.o \
	pieces_pool_test.o \
	piece_progress.o \
	big_endian_test.o \
	conn_test.o \
	client_test.o \

	@ touch test_obj

src_obj: \
	hash.o \
	tokenizer.o \
	file_buf.o \
	torrent_file.o \
	peer.o \
	tracker_response.o \
	handshake_msg.o \
	message.o \
	bitfield.o \
	pieces_pool.o \
	piece_progress.o \
	big_endian.o \
	conn.o \
	client.o \

	@ touch src_obj

%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS_ALL) -c $<

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS_ALL) -c $<

clean:
	@ $(RM) $(TARGET) $(TARGET_TEST) *.o src_obj test_obj

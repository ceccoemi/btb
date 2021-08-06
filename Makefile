TARGET := btb
TARGET_TEST := btb-test
SRC_DIR := src
TEST_DIR := test
TARGET_MAIN := $(SRC_DIR)/main.c
TARGET_TEST_MAIN := $(TEST_DIR)/main.c
VPATH = $(SRC_DIR) $(TEST_DIR)

CC := gcc
WARNINGS := -Wall -Werror
CSTD := c17
CFLAGS_ALL := -std=$(CSTD) $(WARNINGS)
CFLAGS_TEST := -g
CFLAGS_RELEASE := -O3
LIBS := -lcrypto -lcurl -lpthread -lm
VALGRIND_SUPP_FILE := valgrind.supp
VALGRIND_CMD := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=$(VALGRIND_SUPP_FILE)

.PHONY: clean test

all: test build

build: $(TARGET_MAIN) src_obj
	$(CC) $(CFLAGS_ALL) $(CFLAGS_RELEASE) -o $(TARGET) *.o $< $(LIBS)

test: $(TARGET_TEST_MAIN) src_obj test_obj
	$(CC) $(CFLAGS_ALL) $(CFLAGS_TEST) -o $(TARGET_TEST) *.o $< $(LIBS)
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

%.o: %.c
	$(CC) $(CFLAGS_ALL) -c $<

clean:
	@ $(RM) $(TARGET) $(TARGET_TEST) *.o src_obj test_obj

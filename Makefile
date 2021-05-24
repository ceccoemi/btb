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

test: tokenizer.o tokenizer_test.o file_buf.o file_buf_test.o torrent_file.o torrent_file_test.o peer.o peer_test.o tracker_response.o tracker_response_test.o handshake.o handshake_test.o message.o message_test.o bitfield.o bitfield_test.o pieces_queue.o pieces_queue_test.o $(TEST_DIR)/main.c
	@ $(CC) $(CFLAGS) -o $(TARGET_TEST) $? $(LIBS)
	@ ./$(TARGET_TEST)

pieces_queue_test.o: $(TEST_DIR)/pieces_queue_test.c
	@ $(CC) $(CFLAGS) -c $?

pieces_queue.o: $(SRC_DIR)/pieces_queue.c
	@ $(CC) $(CFLAGS) -c $?

bitfield_test.o: $(TEST_DIR)/bitfield_test.c
	@ $(CC) $(CFLAGS) -c $?

bitfield.o: $(SRC_DIR)/bitfield.c
	@ $(CC) $(CFLAGS) -c $?

message_test.o: $(TEST_DIR)/message_test.c
	@ $(CC) $(CFLAGS) -c $?

message.o: $(SRC_DIR)/message.c
	@ $(CC) $(CFLAGS) -c $?

handshake_test.o: $(TEST_DIR)/handshake_test.c
	@ $(CC) $(CFLAGS) -c $?

handshake.o: $(SRC_DIR)/handshake.c
	@ $(CC) $(CFLAGS) -c $?

tracker_response_test.o: $(TEST_DIR)/tracker_response_test.c
	@ $(CC) $(CFLAGS) -c $?

tracker_response.o: $(SRC_DIR)/tracker_response.c
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

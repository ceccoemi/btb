#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MSG_LEN_BYTES 4
#define MSG_ID_BYTES 1

#define MSG_CHOKE 0
#define MSG_UNCHOKE 1
#define MSG_INTERESTED 2
#define MSG_NOTINTERESTED 3
#define MSG_HAVE 4
#define MSG_BITFIELD 5
#define MSG_REQUEST 6
#define MSG_PIECE 7
#define MSG_CANCEL 8

#define MESSAGE_RECEIVE_TIMEOUT_MSEC 10000
#define MESSAGE_SEND_TIMEOUT_MSEC 10000

typedef struct message
{
  uint8_t id;
  size_t payload_len;
  unsigned char* payload;
} message;

// Read a message from a socket
message* read_message(int);

message* create_message(uint8_t msg_id, size_t payload_len, unsigned char* payload);

// Send a message on a socket.
// It returns true if it succeeds.
bool send_message(int, message*);

void free_message(message*);

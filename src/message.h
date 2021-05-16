#pragma once

#include <stdint.h>
#include <stdlib.h>

#define MSG_CHOKE 0
#define MSG_UNCHOKE 1
#define MSG_INTERESTED 2
#define MSG_NOTINTERESTED 3
#define MSG_HAVE 4
#define MSG_BITFIELD 5
#define MSG_REQUEST 6
#define MSG_PIECE 7
#define MSG_CANCEL 8

typedef struct message
{
  uint8_t id;
  size_t payload_len;
  char* payload;
} message;

// Read a message from a socket
message* read_message(int socketfd);

void free_message(message*);

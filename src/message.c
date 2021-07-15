#include "message.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "big_endian.h"

message* init_message(uint8_t msg_id, size_t payload_len, const char* payload)
{
  message* msg = malloc(sizeof(message));
  msg->id = msg_id;
  msg->payload_len = payload_len;
  msg->payload = NULL;
  if (msg->payload_len > 0) {
    msg->payload = malloc(payload_len);
    memcpy(msg->payload, payload, msg->payload_len);
  }
  return msg;
}

void free_message(message* msg)
{
  if (msg == NULL) return;
  free(msg->payload);
  free(msg);
}

message_encoded* encode_message(message* msg)
{
  message_encoded* encoded = malloc(sizeof(message_encoded));
  encoded->size = MSG_LEN_BYTES + MSG_ID_BYTES + msg->payload_len;
  encoded->buf = malloc(sizeof(encoded->size));
  char* last = encoded->buf;
  bool ok = lu_to_big_endian(encoded->size - MSG_LEN_BYTES, (unsigned char*)last, MSG_LEN_BYTES);
  if (!ok) {
    fprintf(stderr, "lu_to_big_endian failed\n");
    free_message_encoded(encoded);
    return NULL;
  }
  last += MSG_LEN_BYTES;
  last[0] = msg->id;
  last++;
  memcpy(last, msg->payload, msg->payload_len);
  return encoded;
}

void free_message_encoded(message_encoded* encoded)
{
  if (encoded == NULL) return;
  free(encoded->buf);
  free(encoded);
}

message* decode_message(char* buf, size_t buf_length)
{
  unsigned long message_length = big_endian_to_lu((unsigned char*)buf, MSG_LEN_BYTES);
  if ((message_length + MSG_LEN_BYTES) != buf_length) {
    fprintf(stderr, "buffer size (%lu) doesn't match message length + %d (%lu)\n", buf_length,
            MSG_LEN_BYTES, message_length + MSG_LEN_BYTES);
    return NULL;
  }
  return init_message(buf[MSG_LEN_BYTES], message_length - MSG_ID_BYTES,
                      buf + MSG_LEN_BYTES + MSG_ID_BYTES);
}
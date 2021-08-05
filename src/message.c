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
#include "conn.h"
#include "defer.h"

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
  encoded->buf = malloc(encoded->size);
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
bool send_message_to_conn(message* msg, conn* c, int timeout_sec)
{
  message_encoded* msg_encoded = encode_message(msg);
  if (msg_encoded == NULL) {
    fprintf(stderr, "encode_message failed\n");
    return false;
  }
  DEFER({ free_message_encoded(msg_encoded); });
  bool ok = send_data(c, msg_encoded->buf, msg_encoded->size, timeout_sec);
  if (!ok) {
    fprintf(stderr, "send_data failed\n");
    return false;
  }
  return true;
}

message* read_message_from_conn(conn* c, int timeout_sec)
{
  size_t buf_size = MSG_LEN_BYTES + MSG_ID_BYTES;
  char buf[buf_size];
  // fprintf(stdout, "reading first %lu bytes of the message\n", buf_size);
  int bytes_received = receive_data(c, buf, buf_size, timeout_sec);
  if (bytes_received <= 0) {
    fprintf(stderr, "receive_data failed\n");
    return NULL;
  }
  if (bytes_received != buf_size) {
    fprintf(stderr, "receive_data failed: expected %lu bytes, got %d bytes instead\n", buf_size,
            bytes_received);
    return NULL;
  }
  unsigned long payload_len_plus_msg_id_len = big_endian_to_lu((unsigned char*)buf, MSG_LEN_BYTES);
  if (payload_len_plus_msg_id_len == 0) {
    fprintf(stderr, "got invalid msg len and id:");
    for (size_t i = 0; i < MSG_LEN_BYTES + MSG_ID_BYTES; i++) {
      fprintf(stderr, " %d", (unsigned char)buf[i]);
    }
    fprintf(stderr, "\n");
    return NULL;
  }
  unsigned long payload_len = payload_len_plus_msg_id_len - MSG_ID_BYTES;
  uint8_t msg_id = buf[MSG_LEN_BYTES];
  // fprintf(stdout, "got msg ID=%hu and an expected payload of %lu bytes\n", msg_id, payload_len);
  char* payload_buf = malloc(payload_len);
  DEFER({ free(payload_buf); });
  if (payload_len > 0) {
    // fprintf(stdout, "reading message payload\n");
    int bytes_received = 0;
    while (bytes_received < payload_len) {
      int received =
          receive_data(c, payload_buf + bytes_received, payload_len - bytes_received, timeout_sec);
      if (received <= 0) {
        fprintf(stderr, "receive_data failed\n");
        return NULL;
      }
      // fprintf(stdout, "received %d bytes of message payload\n", received);
      bytes_received += received;
    }
  }
  return init_message(msg_id, payload_len, payload_buf);
}
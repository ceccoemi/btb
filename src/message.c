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

message* read_message(int sockfd)
{
  // TODO: custom buf size
  unsigned char buf[2048];
  int bytes_received;
  for (int i = 0; i < 5; i++) {
    bytes_received = recv(sockfd, buf, 2048, 0);
    fprintf(stdout, "RECEIVED %d BYTES\n", bytes_received);
  }
  if (bytes_received < 0) {
    fprintf(stderr, "recv failed: %s\n", strerror(errno));
    return NULL;
  }
  if (bytes_received == 0) {
    fprintf(stderr, "connection lost with peer\n");
    return NULL;
  }
  if (bytes_received < 5) {
    fprintf(stderr, "unknown data from peer\n");
    return NULL;
  }
  /*
  if (buf[0] == 255 && buf[1] == 255 && buf[2] == 255 && buf[3] == 255) {
    fprintf(stderr, "unknown data from peer\n");
    return NULL;
  }
  */
  size_t message_length = big_endian_to_lu(buf, 4);
  uint8_t message_id = buf[4];
  if ((unsigned int)bytes_received < message_length + 4) {
    fprintf(stderr, "bytes received (%d) are less than message length (%lu)\n", bytes_received,
            message_length);
    return NULL;
  }
  message* m = malloc(sizeof(message));
  m->id = message_id;
  m->payload_len = message_length - 1;  // subtract the message ID
  m->payload = malloc(m->payload_len);
  memcpy(m->payload, buf + 5, m->payload_len);
  return m;
}

message* create_message(uint8_t msg_id, size_t payload_len, unsigned char* payload)
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

bool send_message(int sockfd, message* msg)
{
  bool ok = true;

  struct pollfd pfds[1];
  pfds[0].fd = sockfd;
  pfds[0].events = POLLOUT;
  int num_events = poll(pfds, 1, MESSAGE_SEND_TIMEOUT_MSEC);
  if (num_events == 0) {
    fprintf(stderr, "timed out\n");
    return -1;
  }
  int happened = pfds[0].revents & POLLOUT;

  if (!happened) {
    fprintf(stderr, "unexpected event: %d\n", happened);
    ok = false;
    return ok;
  }

  size_t msg_data_len = MSG_LEN_BYTES + MSG_ID_BYTES + msg->payload_len;
  unsigned char* msg_data = malloc(msg_data_len);
  lu_to_big_endian(1 + msg->payload_len, msg_data, MSG_LEN_BYTES);
  msg_data[4] = msg->id;
  memcpy(msg_data + MSG_LEN_BYTES + MSG_ID_BYTES, msg->payload, msg->payload_len);
  int bytes_sent = send(sockfd, msg_data, msg_data_len, 0);
  free(msg_data);
  if (bytes_sent != msg_data_len) {
    ok = false;
  }
  return ok;
}

void free_message(message* m)
{
  if (m != NULL) {
    if (m->payload != NULL) free(m->payload);
    free(m);
  }
}

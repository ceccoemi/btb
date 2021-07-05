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

message* init_message(uint8_t msg_id, size_t payload_len, char* payload)
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

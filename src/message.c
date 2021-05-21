#include "message.h"

#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

message* read_message(int sockfd)
{
  struct pollfd pfds[1];
  pfds[0].fd = sockfd;
  pfds[0].events = POLLIN;
  int num_events = poll(pfds, 1, 10000);  // 10 seconds timeout
  if (num_events == 0) {
    fprintf(stderr, "Timed out\n");
    return NULL;
  }
  int happened = pfds[0].revents & POLLIN;

  if (!happened) {
    fprintf(stderr, "unexpected event: %d\n", happened);
    return NULL;
  }
  unsigned char buf[2048];
  int bytes_received = recv(sockfd, buf, 2048, 0);
  if (bytes_received < 5) {
    fprintf(stderr, "received less than 5 bytes from peer\n");
    return NULL;
  }
  if (buf[0] == 255 && buf[1] == 255 && buf[2] == 255 && buf[3] == 255) {
    fprintf(stderr, "unknown data from peer\n");
    return NULL;
  }
  size_t message_length = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
  uint8_t message_id = buf[4];
  if ((unsigned int)bytes_received < message_length + 4) {
    fprintf(stderr, "bytes received (%d) are less than message length (%ld)\n", bytes_received,
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

void free_message(message* m)
{
  if (m != NULL) {
    if (m->payload != NULL) free(m->payload);
    free(m);
  }
}

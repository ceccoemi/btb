#define _POSIX_C_SOURCE 200112L

#include "handshake.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "peer.h"

handshake_msg *init_handshake_msg(const char peer_id[PEER_ID_LENGTH],
                                  const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  handshake_msg *h = malloc(sizeof(handshake_msg));
  int pstrlen = 19;  // string length of the <pstr> field.
  int handshake_length = 49 + pstrlen;
  h->msg = malloc(handshake_length);
  char *handshake_last = h->msg;
  handshake_last[0] = pstrlen;  // protocol identifier length, always 19
  handshake_last++;
  char pstr[] = "BitTorrent protocol";
  memcpy(handshake_last, pstr, pstrlen);
  handshake_last += pstrlen;
  // 8 reserved bytes all set to 0
  for (int i = 0; i < 8; i++) {
    handshake_last[i] = 0;
  }
  handshake_last += 8;
  // Add info_hash
  h->_info_hash_start = handshake_last;
  for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    handshake_last[i] = info_hash[i];
  }
  handshake_last += SHA_DIGEST_LENGTH;
  // Add peer_id
  for (int i = 0; i < PEER_ID_LENGTH; i++) {
    handshake_last[i] = peer_id[i];
  }
  handshake_last += PEER_ID_LENGTH;
  h->length = handshake_last - h->msg;
  return h;
}

void free_handshake_msg(handshake_msg *h)
{
  if (h != NULL) {
    if (h->msg != NULL) free(h->msg);
    free(h);
  }
}

int perform_handshake(peer *p, handshake_msg *h)
{
  int return_socketfd = 0;

  char peer_addr[16];  // max len ip address is xxx.xxx.xxx.xxx (15 + '\0' = 16 bytes)
  sprintf(peer_addr, "%d.%d.%d.%d", p->address[0], p->address[1], p->address[2], p->address[3]);
  char peer_port[6];  // maximum port number value is 5 digits
  sprintf(peer_port, "%hu", p->port);

  struct addrinfo *hints = calloc(1, sizeof(struct addrinfo));
  hints->ai_family = AF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
  struct addrinfo *res;
  getaddrinfo(peer_addr, peer_port, hints, &res);

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  fcntl(sockfd, F_SETFL, O_NONBLOCK);  // non-blocking socket

  fprintf(stdout, "contacting peer %s on port %s\n", peer_addr, peer_port);
  int err = connect(sockfd, res->ai_addr, res->ai_addrlen);
  if (errno != EINPROGRESS) {
    fprintf(stdout, "connection error: %d\n", err);
    return_socketfd = -1;
    goto exit;
  }

  struct pollfd pfds[1];
  pfds[0].fd = sockfd;
  pfds[0].events = POLLOUT;
  int num_events = poll(pfds, 1, HANDSHAKE_TIMEOUT_MSEC);
  if (num_events == 0) {
    fprintf(stderr, "timed out\n");
    return_socketfd = -1;
    goto exit;
  }
  int happened = pfds[0].revents & POLLOUT;

  if (!happened) {
    fprintf(stderr, "unexpected event: %d\n", happened);
    return_socketfd = -1;
    goto exit;
  }

  int bytes_sent = send(sockfd, h->msg, h->length, 0);
  if (bytes_sent < 0) {
    fprintf(stderr, "send failed: %s\n", strerror(errno));
    return_socketfd = -1;
    goto exit;
  }
  if (bytes_sent != (int)h->length) {
    fprintf(stderr, "didn't sent all handshake data: sent %d, want %ld\n", bytes_sent, h->length);
    return_socketfd = -1;
    goto exit;
  }

  pfds[0].events = POLLIN;
  num_events = poll(pfds, 1, HANDSHAKE_TIMEOUT_MSEC);
  if (num_events == 0) {
    fprintf(stderr, "timed out\n");
    return_socketfd = -1;
    goto exit;
  }
  happened = pfds[0].revents & POLLIN;

  if (!happened) {
    fprintf(stderr, "unexpected event: %d\n", happened);
    return_socketfd = -1;
    goto exit;
  }

  char *buf_response = malloc(h->length);
  int bytes_received = recv(sockfd, buf_response, h->length, 0);
  if (bytes_received < 0) {
    free(buf_response);
    fprintf(stderr, "recv failed: %s\n", strerror(errno));
    return_socketfd = -1;
    goto exit;
  }
  if (bytes_received != (int)h->length) {
    free(buf_response);
    fprintf(stderr, "unexpected peer response: got %d bytes, want %ld bytes\n", bytes_received,
            h->length);
    return_socketfd = -1;
    goto exit;
  }
  int info_hash_start = h->_info_hash_start - h->msg;
  if (memcmp(buf_response + info_hash_start, h->_info_hash_start, SHA_DIGEST_LENGTH) != 0) {
    free(buf_response);
    fprintf(stderr, "wrong infohash from peer response\n");
    return_socketfd = -1;
    goto exit;
  }
  free(buf_response);
  fprintf(stdout, "performed handshake with peer %s on port %s\n", peer_addr, peer_port);
  return_socketfd = sockfd;

exit:
  freeaddrinfo(hints);
  freeaddrinfo(res);
  return return_socketfd;
}


#define _POSIX_C_SOURCE 200112L

#include "handshake.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "peer.h"

int perform_handshake(tracker_response *r, const unsigned char peer_id[PEER_ID_LENGTH],
                      const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  int pstrlen = 19;  // string length of the <pstr> field.
  int handshake_length = 49 + pstrlen;
  char handshake_request[handshake_length];
  char *handshake_last = handshake_request;
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
  for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    handshake_last[i] = info_hash[i];
  }
  handshake_last += SHA_DIGEST_LENGTH;
  // Add peer_id
  for (int i = 0; i < PEER_ID_LENGTH; i++) {
    handshake_last[i] = peer_id[i];
  }
  handshake_last += PEER_ID_LENGTH;

  for (int i = 0; i < r->num_peers; i++) {
    if (contact_peer(r->peers[i], handshake_request, handshake_length, pstrlen) == 0) {
      break;
    }
  }
}

int contact_peer(peer *p, char *handshake_request, int handshake_length, int pstrlen)
{
  int return_code = 0;

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

  fprintf(stdout, "connecting %s:%s\n", peer_addr, peer_port);
  int err = connect(sockfd, res->ai_addr, res->ai_addrlen);
  if (errno != EINPROGRESS) {
    fprintf(stdout, "connection error: %d\n", err);
    return_code = -1;
    goto exit;
  }

  struct pollfd pfds[1];
  pfds[0].fd = sockfd;
  pfds[0].events = POLLOUT;
  int num_events = poll(pfds, 1, 5000);  // 5 seconds timeout
  if (num_events == 0) {
    fprintf(stderr, "Poll timed out\n");
    return_code = -1;
    goto exit;
  }
  int happened = pfds[0].revents & POLLOUT;

  if (!happened) {
    fprintf(stderr, "unexpected event: %d\n", happened);
    return_code = -1;
    goto exit;
  }
  fprintf(stdout, "ready to send data\n");

  int bytes_sent = send(sockfd, handshake_request, handshake_length, 0);
  if (bytes_sent != handshake_length) {
    fprintf(stderr, "didn't sent all handshake data: sent %d, want %d\n", bytes_sent,
            handshake_length);
    return_code = -1;
    goto exit;
  }
  fprintf(stdout, "sent %d bytes\n", bytes_sent);

  pfds[0].events = POLLIN;
  num_events = poll(pfds, 1, 5000);  // 5 seconds timeout
  if (num_events == 0) {
    fprintf(stderr, "Poll timed out\n");
    return_code = -1;
    goto exit;
  }
  happened = pfds[0].revents & POLLIN;

  if (!happened) {
    fprintf(stderr, "unexpected event: %d\n", happened);
    return_code = -1;
    goto exit;
  }
  fprintf(stdout, "ready to receive data\n");

  char buf_response[2048];
  int bytes_received = recv(sockfd, buf_response, 2048, 0);
  fprintf(stdout, "received %d bytes\n", bytes_received);

  if (bytes_received < handshake_length) {
    fprintf(stderr, "unexpected peer response: got %d bytes, want %d bytes\n", bytes_received,
            handshake_length);
    return_code = -1;
    goto exit;
  }
  int info_hash_start = 1 + pstrlen + 8;
  if (memcmp(buf_response + info_hash_start, handshake_request + info_hash_start,
             SHA_DIGEST_LENGTH) != 0) {
    fprintf(stderr, "wrong infohash from peer response\n");
    return_code = -1;
    goto exit;
  }

exit:
  freeaddrinfo(hints);
  freeaddrinfo(res);
  return return_code;
}

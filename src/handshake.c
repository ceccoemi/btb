#define _POSIX_C_SOURCE 200112L

#include "handshake.h"

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "peer.h"

int perform_handshake(peer *p, const unsigned char peer_id[PEER_ID_LENGTH],
                      const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  char handshake_request[2048] = "";
  strcat(handshake_request, "x13");  // protocol identifier length, always 19 (x13 in hex)
  strcat(handshake_request, "BitTorrent protocol");
  strcat(handshake_request,
         "\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00");  // 8 reserved bytes all set to 0
  for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
    char x[5];
    sprintf(x, "\\x%02x", info_hash[i]);
    strcat(handshake_request, x);
  }
  for (int i = 0; i < PEER_ID_LENGTH; i++) {
    char x[5];
    sprintf(x, "\\x%02x", peer_id[i]);
    strcat(handshake_request, x);
  }
  fprintf(stdout, "handshake request:\n%s\n", handshake_request);

  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char peer_addr[64];
  sprintf(peer_addr, "%d.%d.%d.%d", p->address[0], p->address[1], p->address[2], p->address[3]);
  char peer_port[6];  // maximum port number value is 5 digits
  sprintf(peer_port, "%d", p->port);

  fprintf(stdout, "contacting peer at %s:%s\n", peer_addr, peer_port);
  // getaddrinfo(peer_addr, peer_port, &hints, &res);
  getaddrinfo("www.example.com", "3490", &hints, &res);

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  int err = connect(sockfd, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    fprintf(stdout, "connection error\n");
    return err;
  }
  int bytes_sent = send(sockfd, handshake_request, strlen(handshake_request), 0);
  fprintf(stdout, "bytes_sent: %d\n", bytes_sent);

  return 0;
}

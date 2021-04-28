#define _POSIX_C_SOURCE 200112L

#include "handshake.h"

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "peer.h"

int perform_handshake(tracker_response *r, const unsigned char peer_id[PEER_ID_LENGTH],
                      const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  // char handshake_request[2048] = "";
  // strcat(handshake_request, "x13");  // protocol identifier length, always 19 (x13 in hex)
  // strcat(handshake_request, "BitTorrent protocol");
  // strcat(handshake_request,
  //       "\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00");  // 8 reserved bytes all set to 0
  //// Convert info_hash into hexadecimal
  // for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
  //  char x[5];
  //  sprintf(x, "\\x%02x", info_hash[i]);
  //  strcat(handshake_request, x);
  //}
  // for (int i = 0; i < PEER_ID_LENGTH; i++) {
  //  char x[5];
  //  sprintf(x, "\\x%02x", peer_id[i]);
  //  strcat(handshake_request, x);
  //}
  // fprintf(stdout, "handshake request:\n%s\n", handshake_request);
  int pstrlen = 19;  // string length of the <pstr> field.
  char handshake_request[49 + pstrlen];
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
  fprintf(stdout, "handshake size:\n%d\n", handshake_last - handshake_request);

  // Try to connect to the first peer
  peer *p = r->peers[0];

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

  fprintf(stdout, "connecting %s:%s\n", peer_addr, peer_port);
  int err = connect(sockfd, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    fprintf(stdout, "connection error\n");
    return err;
  }
  int bytes_sent = send(sockfd, handshake_request, handshake_last - handshake_request, 0);
  fprintf(stdout, "bytes_sent: %d\n", bytes_sent);

  char buf[2048];
  int bytes_received = recv(sockfd, buf, 2048, 0);
  fprintf(stdout, "bytes_received: %d\n", bytes_received);
  fprintf(stdout, "peer response: %d%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);

  freeaddrinfo(hints);
  freeaddrinfo(res);
  return 0;
}

#pragma once

#include <stdint.h>

#include "torrent_file.h"

#define PEER_ID_LENGTH 20
#define PEER_BLOB_SIZE 6  // number of bytes to represent a peer

typedef struct
{
  uint16_t port;
  unsigned char address[4];
} peer;

typedef struct
{
  long interval;
  peer **peers;
  long num_peers;
} tracker_response;

tracker_response *contact_tracker(torrent_file *tf, const char peer_id[PEER_ID_LENGTH]);

void free_tracker_response(tracker_response *r);

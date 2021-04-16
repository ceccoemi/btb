#pragma once

#include <stdint.h>

#include "torrent_file.h"

#define PEER_ID_LENGTH 20

typedef struct
{
  uint16_t port;
  char *address;
} peer;

typedef struct
{
  long interval;
  peer *peers;
} tracker_response;

tracker_response *contact_tracker(torrent_file *tf, const char peer_id[PEER_ID_LENGTH]);

void free_tracker_response(tracker_response *r);

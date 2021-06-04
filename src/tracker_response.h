#pragma once

#include "peer.h"
#include "torrent_file.h"

#define PEER_ID_LENGTH 20

typedef struct
{
  long interval;
  peer **peers;
  long num_peers;
} tracker_response;

tracker_response *init_tracker_response();

// Contact the tracker identified in the torrent file and return its response.
int contact_tracker(tracker_response *r, torrent_file *tf, const char peer_id[PEER_ID_LENGTH]);

void free_tracker_response(tracker_response *r);

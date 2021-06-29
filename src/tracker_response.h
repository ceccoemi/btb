#pragma once

#include "peer.h"
#include "torrent_file.h"

typedef struct
{
  long interval;
  peer **peers;
  size_t num_peers;
} tracker_response;

tracker_response *init_tracker_response();

// Contact the tracker identified in the torrent file and return its response.
int contact_tracker(tracker_response *r, torrent_file *tf);

void free_tracker_response(tracker_response *r);

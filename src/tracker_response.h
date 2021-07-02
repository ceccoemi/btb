#pragma once

#include <stdbool.h>

#include "peer.h"
#include "torrent_file.h"

typedef struct
{
  long interval;
  peer **peers;
  size_t num_peers;
} tracker_response;

tracker_response *init_tracker_response();

// Contact the tracker identified in the torrent file and fill the tracker_response struct.
// It returns true if it succeeds.
bool contact_tracker(tracker_response *r, torrent_file *tf);

void free_tracker_response(tracker_response *r);

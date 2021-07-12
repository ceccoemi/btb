#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "peer.h"
#include "torrent_file.h"

typedef struct
{
  long interval;
  peer **peers;
  size_t num_peers;
} tracker_response;

tracker_response *init_tracker_response();

void free_tracker_response(tracker_response *tr);

// Contact the tracker identified in the torrent file and fill the tracker_response struct.
// It returns true if it succeeds.
bool contact_tracker(tracker_response *tr, torrent_file *tf);

#pragma once

#include "torrent_file.h"

#define PEER_ID_LENGTH 20

char *contact_tracker(torrent_file *tf, const char peer_id[PEER_ID_LENGTH]);

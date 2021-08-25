#pragma once

#include <pthread.h>
#include <stddef.h>

#include "piece_progress.h"
#include "torrent_file.h"

// Thread-safe data structure that holds the final file to be downloaded.
typedef struct
{
  // Buffer that holds the pieces.
  char **pieces_buf;
  const torrent_file *tf;
  pthread_mutex_t _mutex;
} downloaded_file;

downloaded_file *init_downloaded_file(const torrent_file *tf);

void free_downloaded_file(downloaded_file *df);

// Add a downloaded piece to the final output file.
bool add_piece(downloaded_file *df, const piece_progress *pp);

// Write all the pieces in the output file specified in the torrent file.
// It returns false if all the pieces weren't downloaded.
bool write_to_file(downloaded_file *df);
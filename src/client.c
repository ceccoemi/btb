#include "client.h"

#define _GNU_SOURCE
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "peer.h"
#include "peer_id.h"
#include "piece_progress.h"
#include "pieces_pool.h"
#include "tokenizer.h"
#include "torrent_file.h"
#include "tracker_response.h"

// Data structure to pass to the thread responsible to download the piece.
// The variable "ok" must be set by the thread to signal success or not.
struct download_piece_data
{
  peer *p;
  size_t piece_index;
  torrent_file *tf;
  bool ok;
};

void *download_piece_from_peer(void *data)
{
  struct download_piece_data *d = (struct download_piece_data *)data;
  d->ok = handshake_peer(d->p, d->tf->info_hash);
  if (!d->ok) {
    fprintf(stderr, "handshake_peer failed\n");
    goto exit;
  }
  d->ok = receive_bitfield(d->p);
  if (!d->ok) {
    fprintf(stderr, "receive_bitfield failed\n");
    goto exit;
  }

exit:
  return NULL;
}

void download_torrent(char *torrent_file_path)
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, torrent_file_path);
  if (err != TORRENT_OK) {
    fprintf(stderr, "parse_torrent_file failed\n");
    free_torrent_file(tf);
    return;
  }
  fprintf(stdout, "file name: %s\n", tf->name);
  fprintf(stdout, "pieces number: %lld\n", tf->num_pieces);
  fprintf(stdout, "piece size (bytes): %lld\n", tf->piece_length);
  fprintf(stdout, "\n");

  bool ok = false;
  tracker_response *r = init_tracker_response();
  fprintf(stdout, "contacting tracker %s\n", tf->announce);
  ok = contact_tracker(r, tf);
  if (!ok) {
    fprintf(stderr, "contact_tracker failed\n");
    free_torrent_file(tf);
    free_tracker_response(r);
    return;
  }
  fprintf(stdout, "tracker replied with %lu peers\n", r->num_peers);
  fprintf(stdout, "\n");
  pieces_pool *pool = init_pieces_pool(tf->num_pieces);
  int TEMP = 0;
  while (!is_done(pool)) {
    size_t piece_index = get_piece_index(pool);
    fprintf(stdout, "trying to download piece #%lu\n", piece_index);
    bool piece_downloaded = false;
    for (long i = 0; i < r->num_peers; i++) {
      struct download_piece_data d;
      d.p = r->peers[i];
      d.piece_index = piece_index;
      d.tf = tf;
      d.ok = false;

      pthread_t thread_id;
      pthread_create(&thread_id, NULL, download_piece_from_peer, &d);

      struct timespec ts;
      if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        fprintf(stderr, "clock_gettime failed\n");
        goto exit;
      }
      ts.tv_sec += 5;  // Timeout in seconds

      int err = pthread_timedjoin_np(thread_id, NULL, &ts);
      if (err != 0) {
        fprintf(stdout, "timeout expired, aborting\n");
        pthread_cancel(thread_id);
        continue;
      }
      if (!d.ok) {
        fprintf(stderr, "download_piece_from_peer failed\n");
        continue;
      }
      piece_downloaded = true;
      break;
    }
    if (!piece_downloaded) {
      mark_as_undone(pool, piece_index);
    } else {
      fprintf(stdout, "piece %lu download with success!\n\n", piece_index);
    }
    TEMP++;
    if (TEMP >= 2) {
      break;
    }
  }

exit:
  free_pieces_pool(pool);
  free_tracker_response(r);
  free_torrent_file(tf);
}

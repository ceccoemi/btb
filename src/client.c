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

#define PIECE_DOWNLOAD_OK 0
#define PIECE_DOWNLOAD_FAIL 1

struct download_piece_data
{
  peer *p;
  size_t piece_index;
  torrent_file *tf;
};

void *download_piece_from_peer(void *data)
{
  struct download_piece_data *d = (struct download_piece_data *)data;
  bool ok;
  ok = handshake_peer(d->p, d->tf->info_hash);
  if (!ok) {
    fprintf(stderr, "handshake_peer failed\n");
    goto exit;
  }

exit:
  pthread_exit(NULL);
  return NULL;
}

void download_torrent(char *torrent_file_path)
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, torrent_file_path);
  if (err != TORRENT_OK) {
    fprintf(stderr, "parse_torrent_file failed\n");
    goto exit;
  }
  fprintf(stdout, "piece_length: %lld\n", tf->piece_length);

  tracker_response *r = init_tracker_response();
  int n = contact_tracker(r, tf);
  if (n != 0) {
    fprintf(stderr, "contact_tracker failed\n");
    goto exit;
  }
  pieces_pool *pool = init_pieces_pool(tf->num_pieces);
  while (!is_done(pool)) {
    size_t piece_index = get_piece_index(pool);
    // NOTE: try with the firsts 10 peers
    for (long i = 0; i < r->num_peers && i < 10; i++) {
      struct download_piece_data d;
      d.p = r->peers[i];
      d.piece_index = piece_index;
      d.tf = tf;

      pthread_t thread_id;
      pthread_create(&thread_id, NULL, download_piece_from_peer, &d);

      struct timespec ts;
      if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        fprintf(stderr, "clock_gettime failed\n");
        goto exit;
      }
      ts.tv_sec += 5;  // Timeout 5 seconds

      int err = pthread_timedjoin_np(thread_id, NULL, &ts);
      if (err != 0) {
        fprintf(stdout, "timeout expired for thread %lu, aborting\n", thread_id);
        pthread_cancel(thread_id);
        mark_as_undone(pool, piece_index);
      }
    }
  }

exit:
  free_pieces_pool(pool);
  free_tracker_response(r);
  free_torrent_file(tf);
}

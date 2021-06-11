#include "client.h"

#include <stdbool.h>
#include <stdio.h>

#include "peer.h"
#include "pieces_queue.h"
#include "tokenizer.h"
#include "torrent_file.h"
#include "tracker_response.h"

void download_torrent(char *torrent_file_path)
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, torrent_file_path);
  if (err != TORRENT_OK) {
    fprintf(stderr, "parse_torrent_file failed\n");
    goto exit;
  }

  char my_peer_id[PEER_ID_LENGTH + 1] = "BTB-14011996ecis2211";

  tracker_response *r = init_tracker_response();
  int n = contact_tracker(r, tf, my_peer_id);
  if (n != 0) {
    fprintf(stderr, "contact_tracker failed\n");
    goto exit;
  }
  pieces_queue *q = init_pieces_queue(tf->num_pieces);
  for (long i = 0; i < r->num_peers; i++) {
    peer *p = r->peers[i];
    int out;
    out = handshake_peer(p, my_peer_id, tf->info_hash);
    if (out != 0) {
      fprintf(stderr, "handshake_peer failed\n");
      continue;
    }
    out = receive_bitfield(p);
    if (out != 0) {
      fprintf(stderr, "receive_bitfield failed\n");
      continue;
    }
    out = send_interested(p);
    if (out != 0) {
      fprintf(stderr, "send_interested failed\n");
      continue;
    }
    while (!is_done(q)) {
      size_t piece_index = get_piece_index(q);
      bool success = download_piece(p, piece_index, tf->piece_hashes[piece_index]);
      if (!success) {
        fprintf(stderr, "download_piece failed\n");
        continue;
      }
      mark_as_done(q, piece_index);
    }
    break;  // For now, stop here
  }

exit:
  free_pieces_queue(q);
  free_tracker_response(r);
  free_torrent_file(tf);
}

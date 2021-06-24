#include "client.h"

#include <stdbool.h>
#include <stdio.h>

#include "peer.h"
#include "piece_progress.h"
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
  fprintf(stdout, "piece_length: %lld\n", tf->piece_length);

  char my_peer_id[PEER_ID_LENGTH + 1] = "BTB-14011996ecis2211";

  tracker_response *r = init_tracker_response();
  int n = contact_tracker(r, tf, my_peer_id);
  if (n != 0) {
    fprintf(stderr, "contact_tracker failed\n");
    goto exit;
  }
  pieces_queue *q = init_pieces_queue(tf->num_pieces);
  // for (long i = 0; i < r->num_peers; i++) {
  //  peer *p = r->peers[i];
  //  bool ok;
  //  ok = handshake_peer(p, my_peer_id, tf->info_hash);
  //  if (!ok) {
  //    fprintf(stderr, "handshake_peer failed\n");
  //    continue;
  //  }
  //  ok = receive_bitfield(p);
  //  if (!ok) {
  //    fprintf(stderr, "receive_bitfield failed\n");
  //    continue;
  //  }
  //  ok = send_interested(p);
  //  if (!ok) {
  //    fprintf(stderr, "send_interested failed\n");
  //    continue;
  //  }
  //  // while (!is_done(q)) {
  //  size_t piece_index = get_piece_index(q);
  //  piece_progress *pp = init_piece_progress(piece_index, tf->piece_length);
  //  ok = download_piece(p, pp, tf->piece_hashes[piece_index]);
  //  if (!ok) {
  //    fprintf(stderr, "download_piece failed\n");
  //    free_piece_progress(pp);
  //    goto exit;
  //  }
  //  // TODO: write pp->buf into output file
  //  free_piece_progress(pp);
  //  mark_as_done(q, piece_index);
  //  //}
  //  break;  // For now, stop here
  //}

exit:
  free_pieces_queue(q);
  free_tracker_response(r);
  free_torrent_file(tf);
}

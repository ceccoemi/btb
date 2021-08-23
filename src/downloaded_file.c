#include "downloaded_file.h"

#include <stdio.h>
#include <string.h>

#include "defer.h"
#include "file_buf.h"

downloaded_file *init_downloaded_file(const torrent_file *tf)
{
  downloaded_file *df = malloc(sizeof(downloaded_file));
  df->tf = tf;
  df->pieces_buf = malloc(sizeof(char *) * tf->num_pieces);
  for (size_t i = 0; i < tf->num_pieces; i++) {
    // When a piece is not added it is NULL
    df->pieces_buf[i] = NULL;
  }
  pthread_mutex_init(&df->_mutex, NULL);
  return df;
}

void free_downloaded_file(downloaded_file *df)
{
  pthread_mutex_lock(&df->_mutex);
  if (df == NULL) return;
  for (size_t i = 0; i < df->tf->num_pieces; i++) {
    free(df->pieces_buf[i]);
  }
  free(df->pieces_buf);
  pthread_mutex_unlock(&df->_mutex);
  pthread_mutex_destroy(&df->_mutex);
  free(df);
}

bool add_piece(downloaded_file *df, const piece_progress *pp)
{
  pthread_mutex_lock(&df->_mutex);
  DEFER({ pthread_mutex_unlock(&df->_mutex); });
  df->pieces_buf[pp->index] = malloc(df->tf->piece_length);
  memcpy(df->pieces_buf[pp->index], pp->buf, pp->size);
  return true;
}

bool has_all_pieces(downloaded_file *df)
{
  for (size_t i = 0; i < df->tf->num_pieces; i++) {
    if (df->pieces_buf[i] == NULL) {
      return false;
    }
  }
  return true;
}

bool write_to_file(downloaded_file *df)
{
  pthread_mutex_lock(&df->_mutex);
  DEFER({ pthread_mutex_unlock(&df->_mutex); });
  if (!has_all_pieces(df)) {
    fprintf(stderr, "can't write output file: there are some pieces that must be added\n");
    return false;
  }
  char *data = malloc(df->tf->length);
  DEFER({ free(data); });
  for (size_t i = 0; i < df->tf->num_pieces; i++) {
    // NOTE: assuming that all pieces has same size!!
    memcpy(data + i * df->tf->num_pieces, df->pieces_buf[i], df->tf->piece_length);
  }
  file_buf *fb = init_file_buf(data, df->tf->length);
  DEFER({ free_file_buf(fb); });
  return write_file(fb, df->tf->name);
}
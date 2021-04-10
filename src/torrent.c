#include "torrent.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bencode.h"

torrent *init_torrent()
{
  torrent *t = malloc(sizeof(torrent));
  t->announce = NULL;
  t->comment = NULL;
  t->info_hash = malloc(20 * sizeof(char *));     // SHA-1 hash is 20-byte-long
  t->piece_hashes = malloc(20 * sizeof(char *));  // SHA-1 hash is 20-byte-long
  t->piece_length = 0;
  t->length = 0;
  t->name = NULL;
  return t;
}

int parse_torrent_file(torrent *tr, const char *fname)
{
  // Read the entire file in a buffer.
  // This fails if the file is > 4GB, but it's not our case.
  FILE *f = fopen(fname, "rb");
  if (f == NULL) {
    fprintf(stderr, "Error in opening the file %s\n", fname);
    return TORRENT_ERROR;
  }
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = malloc(length);
  long r = fread(buffer, 1, length, f);
  if (r != length) {
    fprintf(stderr, "Error: read %ld of %ld bytes from file %s\n", r, length,
            fname);
    free(buffer);
    return TORRENT_ERROR;
  }
  fclose(f);

  tokenizer *tk = init_tokenizer(buffer, length);
  free(buffer);

  int exit_code = TORRENT_OK;
  while (true) {
    int err = next(tk);
    if (err == TOKENIZER_END) {
      break;
    }
    if (err == TOKENIZER_MALFORMED_STRING) {
      fprintf(stderr, "tokenizer failed with code %d\n", err);
      exit_code = TORRENT_ERROR;
      break;
    }
    if (memcmp(tk->token, "announce", tk->token_size) == 0) {
      /* --- parse announce --- */
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "s", tk->token_size) != 0) {
        fprintf(stderr, "expected s token, got %s\n", tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      tr->announce = realloc(tr->announce, tk->token_size);
      memcpy(tr->announce, tk->token, tk->token_size);
      tr->announce_size = tk->token_size;
      /* ------------------------ */
    }
  }

  free_tokenizer(tk);
  return exit_code;
}

void free_torrent(torrent *t)
{
  free(t->name);
  free(t->piece_hashes);
  free(t->info_hash);
  free(t->comment);
  free(t->announce);
  free(t);
}

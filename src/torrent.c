#include "torrent.h"

#include <stdio.h>
#include <stdlib.h>

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
    fprintf(stderr, "error in opening the file %s\n", fname);
    return TORRENT_ERROR;
  }
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = malloc(length + 1);
  long r = fread(buffer, 1, length, f);
  if (r != length) {
    fprintf(stderr, "Error: read %ld of %ld bytes from file %s\n", r, length,
            fname);
    free(buffer);
    return TORRENT_ERROR;
  }
  buffer[length] = '\0';  // fread doesn't NULL-terminate the string
  fclose(f);

  tokenizer *tk = init_tokenizer(buffer);
  free(buffer);

  int err;

  err = next(tk);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "tokenizer failed with code %d\n", err);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }
  if (strcmp(tk->token, "d") != 0) {
    fprintf(stderr, "expected d token, got %s\n", tk->token);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }

  err = next(tk);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "tokenizer failed with code %d\n", err);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }
  if (strcmp(tk->token, "s") != 0) {
    fprintf(stderr, "expected s token, got %s\n", tk->token);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }

  err = next(tk);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "tokenizer failed with code %d\n", err);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }
  if (strcmp(tk->token, "announce") != 0) {
    fprintf(stderr, "expected announce token, got %s\n", tk->token);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }

  err = next(tk);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "tokenizer failed with code %d\n", err);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }
  if (strcmp(tk->token, "s") != 0) {
    fprintf(stderr, "expected s token, got %s\n", tk->token);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }

  err = next(tk);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "tokenizer failed with code %d\n", err);
    free_tokenizer(tk);
    return TORRENT_ERROR;
  }
  tr->announce = malloc(strlen(tk->token) + 1);
  strcpy(tr->announce, tk->token);

  free_tokenizer(tk);
  return TORRENT_OK;
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

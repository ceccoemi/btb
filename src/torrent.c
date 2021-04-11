#include "torrent.h"

#include <openssl/sha.h>
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
  t->info_hash = malloc(SHA_DIGEST_LENGTH * sizeof(char *));
  t->num_pieces = 0;
  t->piece_hashes = NULL;
  t->piece_length = 0;
  t->length = 0;
  t->name = NULL;
  return t;
}

int parse_torrent_file(torrent *tr, const char *fname)
{
  // Read the entire file in a buffer.
  // This fails if the file is > 4GB, but it's shouldn't be our case.
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
    fprintf(stderr, "Error: read %ld of %ld bytes from file %s\n", r, length, fname);
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
    if (memcmp(tk->token, "announce", strlen("announce")) == 0) {
      /* --- parse announce --- */
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "s", tk->token_size) != 0) {
        fprintf(stderr, "expected s token, got %.*s\n", (int)tk->token_size, tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      tr->announce = realloc(tr->announce, tk->token_size + 1);
      memcpy(tr->announce, tk->token, tk->token_size);
      tr->announce[tk->token_size] = '\0';
      /* ------------------------ */
    } else if (memcmp(tk->token, "comment", strlen("comment")) == 0) {
      /* --- parse comment --- */
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "s", tk->token_size) != 0) {
        fprintf(stderr, "expected s token, got %.*s\n", (int)tk->token_size, tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      tr->comment = realloc(tr->comment, tk->token_size + 1);
      memcpy(tr->comment, tk->token, tk->token_size);
      tr->comment[tk->token_size] = '\0';
      /* ------------------------ */
    } else if (memcmp(tk->token, "name", strlen("name")) == 0) {
      /* --- parse name --- */
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "s", tk->token_size) != 0) {
        fprintf(stderr, "expected s token, got %.*s\n", (int)tk->token_size, tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      tr->name = realloc(tr->name, tk->token_size + 1);
      memcpy(tr->name, tk->token, tk->token_size);
      tr->name[tk->token_size] = '\0';
      /* ------------------------ */
    } else if (memcmp(tk->token, "length", strlen("length")) == 0) {
      /* --- parse length --- */
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "i", tk->token_size) != 0) {
        fprintf(stderr, "expected i token, got %.*s\n", (int)tk->token_size, tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      char *num = malloc(tk->token_size + 1);
      memcpy(num, tk->token, tk->token_size);
      num[tk->token_size] = '\0';
      tr->length = strtoll(num, NULL, 10);
      /* ------------------------ */
    } else if (memcmp(tk->token, "piece length", strlen("piece length")) == 0) {
      /* --- parse piece length --- */
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "i", tk->token_size) != 0) {
        fprintf(stderr, "expected i token, got %.*s\n", (int)tk->token_size, tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      char *num = malloc(tk->token_size + 1);
      memcpy(num, tk->token, tk->token_size);
      num[tk->token_size] = '\0';
      tr->piece_length = strtoll(num, NULL, 10);
      /* ------------------------ */
    } else if (memcmp(tk->token, "pieces", strlen("pieces")) == 0) {
      /* --- parse pieces --- */
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "s", tk->token_size) != 0) {
        fprintf(stderr, "expected s token, got %.*s\n", (int)tk->token_size, tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      tr->num_pieces = tk->token_size / SHA_DIGEST_LENGTH;
      tr->piece_hashes = malloc(tr->num_pieces * sizeof(char *));
      for (long long i = 0; i < tr->num_pieces; i++) {
        tr->piece_hashes[i] = malloc(SHA_DIGEST_LENGTH);
        memcpy(tr->piece_hashes[i], tk->token + SHA_DIGEST_LENGTH * i, SHA_DIGEST_LENGTH);
      }
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

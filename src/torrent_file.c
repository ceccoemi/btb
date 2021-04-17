#include "torrent_file.h"

#include <openssl/sha.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"

#pragma GCC diagnostic ignored "-Wpointer-sign"

torrent_file *init_torrent_file()
{
  torrent_file *t = malloc(sizeof(torrent_file));
  t->announce = NULL;
  t->comment = NULL;
  t->info_hash = NULL;
  t->num_pieces = 0;
  t->piece_hashes = NULL;
  t->piece_length = 0;
  t->length = 0;
  t->name = NULL;
  return t;
}

int parse_torrent_file(torrent_file *tr, const char *fname)
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
    fclose(f);
    free(buffer);
    return TORRENT_ERROR;
  }
  fclose(f);

  tokenizer *tk = init_tokenizer(buffer, length);
  free(buffer);

  int exit_code = TORRENT_OK;
  // Pointer to the location where the info section starts,
  // which is the 'd' character after the 'info' key.
  char *info_start = NULL;
  // Value of the list_dict_stack when the key info is encountered.
  // This value should be checked to identify the end of the info dictionary.
  long info_start_stack_value = 0;
  // Pointer to the location where the info section ends,
  // which is the 'e' character.
  char *info_end = NULL;
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
    } else if (memcmp(tk->token, "info", strlen("info")) == 0) {
      info_start = tk->current;
      info_start_stack_value = tk->list_dict_stack;
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      if (memcmp(tk->token, "d", tk->token_size) != 0) {
        fprintf(stderr, "expected d token, got %.*s\n", (int)tk->token_size, tk->token);
        exit_code = TORRENT_ERROR;
        break;
      }
    } else if (info_start != NULL && info_end == NULL &&
               info_start_stack_value == tk->list_dict_stack &&
               memcmp(tk->token, "e", strlen("e")) == 0) {
      info_end = tk->current - 1;  // the tk->current pointer is one char over the 'e' char
      err = next(tk);
      if (err != TOKENIZER_OK) {
        fprintf(stderr, "tokenizer failed with code %d\n", err);
        exit_code = TORRENT_ERROR;
        break;
      }
      tr->info_hash = malloc(SHA_DIGEST_LENGTH);
      SHA1(info_start, info_end - info_start + 1, tr->info_hash);
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

void free_torrent_file(torrent_file *t)
{
  if (t == NULL) {
    return;
  }
  if (t->name != NULL) free(t->name);
  if (t->piece_hashes != NULL) {
    for (long long i = 0; i < t->num_pieces; i++) {
      if (t->piece_hashes[i] != NULL) free(t->piece_hashes[i]);
    }
    free(t->piece_hashes);
  }
  if (t->info_hash != NULL) free(t->info_hash);
  if (t->comment != NULL) free(t->comment);
  if (t->announce != NULL) free(t->announce);
  free(t);
}

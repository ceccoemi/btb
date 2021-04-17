#include "protocol.h"

#include <curl/curl.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bencode.h"
#include "torrent_file.h"

#pragma GCC diagnostic ignored "-Wpointer-sign"

// Struct where the output of the GET request will be written
struct response_data
{
  char *data;
  size_t len;
};

// Callback function to write the curl output in a response_data
size_t parse_response(void *ptr, size_t size, size_t nmemb, struct response_data *r)
{
  r->data = malloc(nmemb * size);
  memcpy(r->data, ptr, nmemb * size);
  r->len = nmemb;
  return size * nmemb;
}

tracker_response *contact_tracker(torrent_file *tf, const char peer_id[PEER_ID_LENGTH])
{
  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "failed to initialize curl\n");
    return NULL;
  }

  // Build GET request
  char get_request[2048] = "";
  strcat(get_request, tf->announce);
  strcat(get_request, "?");
  strcat(get_request, "&peer_id=");
  strcat(get_request, peer_id);
  strcat(get_request, "&port=6881");
  strcat(get_request, "&uploaded=0");
  strcat(get_request, "&downloaded=0");
  strcat(get_request, "&compact=1");
  char left_param[64];
  sprintf(left_param, "&left=%lld", tf->length);
  strcat(get_request, left_param);
  strcat(get_request, "&info_hash=");
  char *encoded_info_hash = curl_easy_escape(curl, tf->info_hash, SHA_DIGEST_LENGTH);
  strcat(get_request, encoded_info_hash);
  curl_free(encoded_info_hash);

  // Perform the request
  curl_easy_setopt(curl, CURLOPT_URL, get_request);
  struct response_data *response = malloc(sizeof(struct response_data));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_response);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
  CURLcode out_code = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (out_code != CURLE_OK) {
    fprintf(stderr, "error in performing the request, curl error code: %d\n", out_code);
    return NULL;
  }

  // Parse the response into tracker_response
  tracker_response *r = malloc(sizeof(tracker_response));
  r->interval = 0;
  r->peers = NULL;
  tokenizer *tk = init_tokenizer(response->data, response->len);
  free(response);
  int err = next(tk);  // d
  if (err != TOKENIZER_OK) goto tokenizer_error;
  err = next(tk);  // s
  if (err != TOKENIZER_OK) goto tokenizer_error;
  err = next(tk);  // "interval" or "peers"
  if (err != TOKENIZER_OK) goto tokenizer_error;
  if (memcmp(tk->token, "interval", tk->token_size) == 0) {
    err = next(tk);  // i
    if (err != TOKENIZER_OK) goto tokenizer_error;
    err = next(tk);  // interval value
    if (err != TOKENIZER_OK) goto tokenizer_error;
    r->interval = strtol(tk->token, NULL, 10);
    err = next(tk);  // e
    if (err != TOKENIZER_OK) goto tokenizer_error;
  }
  free_tokenizer(tk);
  return r;

tokenizer_error:
  fprintf(stderr, "tokenizer error: %d\n", err);
  free(tk);
  free_tracker_response(r);
  return NULL;
}

void free_tracker_response(tracker_response *t)
{
  if (t != NULL) {
    if (t->peers != NULL) {
      free(t->peers);
    }
    free(t);
  }
}

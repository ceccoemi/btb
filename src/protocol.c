#include "protocol.h"

#include <curl/curl.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>

#include "bencode.h"
#include "torrent_file.h"

size_t parse_response(void *ptr, size_t size, size_t nmemb, tracker_response *r)
{
  tokenizer *tk = init_tokenizer(ptr, size * nmemb);
  next(tk);  // d
  next(tk);  // 8
  next(tk);  // interval
  next(tk);  // i
  next(tk);  // interval value
  r->interval = strtol(tk->token, NULL, 10);
  free(tk);
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
  strcat(get_request, "&peer_id=aaaaaaaaaaaaaaaaaaaa");
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

  tracker_response *r = malloc(sizeof(tracker_response));
  curl_easy_setopt(curl, CURLOPT_URL, get_request);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_response);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, r);
  CURLcode res = curl_easy_perform(curl);
  fprintf(stdout, "output code: %d\n", res);
  fprintf(stdout, "interval: %ld\n", r->interval);
  curl_easy_cleanup(curl);
  return r;
}

void free_tracker_response(tracker_response *t)
{
  if (t != NULL) {
    free(t);
  }
}

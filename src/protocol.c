#include "protocol.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "bencode.h"
#include "torrent_file.h"

size_t parse_response(void *ptr, size_t size, size_t nmemb, tracker_response *r)
{
  fprintf(stdout, "%.*s\n", nmemb, ptr);
  tokenizer *tk = init_tokenizer(ptr, size * nmemb);
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
  tracker_response *r;
  fprintf(stdout, "%s\n", tf->announce);
  curl_easy_setopt(curl, CURLOPT_URL, tf->announce);
  // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_response);
  // curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
  CURLcode res = curl_easy_perform(curl);
  fprintf(stdout, "output code: %d\n", res);
  fprintf(stdout, "interval: %d\n", r->interval);
  // free(s);
  curl_easy_cleanup(curl);
  return r;
}

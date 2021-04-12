#include "protocol.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "torrent_file.h"

size_t writefunc(void *ptr, size_t size, size_t nmemb, char **s)
{
  fprintf(stdout, "address of s inside callback: %p\n", s);
  s = malloc(nmemb + 1);
  memcpy(s, ptr, size * nmemb);
  // fprintf(stdout, "got %d bytes\n", nmemb);
  s[nmemb] = '\0';
  // fprintf(stdout, "%.*s\n", nmemb, ptr);
  // fprintf(stdout, "%s\n", s);
  return size * nmemb;
}

char *contact_tracker(torrent_file *tf, const char peer_id[PEER_ID_LENGTH])
{
  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "failed to initialize curl\n");
    return NULL;
  }
  char *s;
  fprintf(stdout, "address of s: %p\n", &s);
  curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
  CURLcode res = curl_easy_perform(curl);
  fprintf(stdout, "%s\n", s);
  fprintf(stdout, "output code: %d\n", res);
  // free(s);
  curl_easy_cleanup(curl);
  return NULL;
}

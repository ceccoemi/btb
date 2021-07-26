#include "client_test.h"

#include <stdbool.h>
#include <stdio.h>

#include "../src/client.h"

void test_client()
{
  bool ok = download_torrent("test/data/debian-10.9.0-amd64-netinst.iso.torrent",
                             "test/data/debian-10.iso");
  if (!ok) {
    fprintf(stderr, "download_torrent failed, expected success\n");
  }
}

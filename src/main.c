#include <stdio.h>

#include "client.h"
#include "version.h"

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stdout, "Usage:\n\n btb <file.torrent>");
  }
  download_torrent(argv[1]);
};

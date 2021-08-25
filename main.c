#include <stdio.h>
#include <string.h>

#include "client.h"
#include "version.h"

static const char help_message[] =
    "btb - BitTorrent Bits\n"
    "\n"
    "A miniminal BitTorrent command line client.\n"
    "\n"
    "Usage: btb <file .torrent>\n";

int main(int argc, char **argv)
{
  if (argc != 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    fprintf(stdout, help_message);
    return 1;
  }
  if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    fprintf(stdout, "btb version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    return 2;
  }
  download_torrent(argv[1]);
  return 0;
};

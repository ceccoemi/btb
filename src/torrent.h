#define TORRENT_OK 0
#define TORRENT_ERROR 1

typedef struct torrent
{
  long int announce_size;
  char *announce;
  char *comment;
  char *info_hash;
  char **piece_hashes;
  int piece_length;
  int length;
  char *name;
} torrent;

// Allocate the memory for a torrent
torrent *init_torrent(void);

// Parse a .torrent file and return an error code.
int parse_torrent_file(torrent *, const char *);

// Free the memory of a torrent
void free_torrent(torrent *);

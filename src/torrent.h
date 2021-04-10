#define TORRENT_OK 0
#define TORRENT_ERROR 1

typedef struct torrent
{
  char *announce;  // null-terminated string
  char *comment;   // null-terminated string
  char *info_hash;
  char **piece_hashes;
  long long piece_length;
  long long length;
  char *name;  // null-terminated string
} torrent;

// Allocate the memory for a torrent
torrent *init_torrent(void);

// Parse a .torrent file and return an error code.
int parse_torrent_file(torrent *, const char *);

// Free the memory of a torrent
void free_torrent(torrent *);

#define TORRENT_OK 0
#define TORRENT_ERROR 1

typedef struct torrent
{
  // URL of the tracker
  char *announce;  // null-terminated string
  char *comment;   // null-terminated string
  // SHA-1 hash of the info section
  unsigned char *info_hash;
  // Size of each piece
  long long piece_length;
  // Number of pieces
  long long num_pieces;
  // SHA-1 hashes of the pieces
  char **piece_hashes;
  // Size of the entire file to download
  long long length;
  // Name of the file to download
  char *name;  // null-terminated string
} torrent;

// Allocate the memory for a torrent
torrent *init_torrent(void);

// Parse a .torrent file, fill the struct and return an error code.
int parse_torrent_file(torrent *, const char *);

// Free the memory of a torrent
void free_torrent(torrent *);

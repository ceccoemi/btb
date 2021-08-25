#include "downloaded_file_test.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/defer.h"
#include "../src/downloaded_file.h"

struct thread_data
{
  size_t index;
  downloaded_file *df;
};

static void *thread_fun(void *d)
{
  struct thread_data *data = (struct thread_data *)d;
  piece_progress *pp = init_piece_progress(data->index, 4);
  DEFER({ free_piece_progress(pp); });
  strcpy(pp->buf, "xx\n");  // Must write 4 bytes
  pp->downloaded = 1;       // One single block
  add_piece(data->df, pp);
  return NULL;
}

void test_downloaded_file()
{
  size_t num_threads = 100;

  const char *output_fname = "test/data/downloaded_file.txt";

  torrent_file *tf = init_torrent_file();
  DEFER({ free_torrent_file(tf); });
  tf->num_pieces = num_threads;
  tf->piece_length = 4;
  tf->length = 4 * num_threads;
  tf->name = malloc(strlen(output_fname) + 1);  // It will be free in free_torrent_file()
  strcpy(tf->name, output_fname);

  downloaded_file *df = init_downloaded_file(tf);
  DEFER({ free_downloaded_file(df); });

  pthread_t thread_ids[num_threads];
  struct thread_data *datas[num_threads];
  for (size_t i = 0; i < num_threads; i++) {
    datas[i] = malloc(sizeof(struct thread_data));
    datas[i]->index = i;
    datas[i]->df = df;
    pthread_create(&thread_ids[i], NULL, thread_fun, datas[i]);
  }
  for (size_t i = 0; i < num_threads; i++) {
    pthread_join(thread_ids[i], NULL);
    free(datas[i]);
  }
  bool ok = write_to_file(df);
  if (!ok) {
    fprintf(stderr, "write_to_file failed\n");
    return;
  }
  if (access(output_fname, F_OK) == -1) {
    fprintf(stderr, "the output file %s doesn't exist\n", output_fname);
  }
}
#include <stdio.h>

#include "bitfield_test.h"
#include "file_buf_test.h"
#include "handshake_test.h"
#include "message_test.h"
#include "peer_test.h"
#include "pieces_queue_test.h"
#include "tokenizer_test.h"
#include "torrent_file_test.h"
#include "tracker_response_test.h"

int main(void)
{
  fprintf(stdout, "Running tests...\n");

  fprintf(stdout, "\ttest_tokenize_int\n");
  test_tokenize_int();
  fprintf(stdout, "\ttest_tokenize_str\n");
  test_tokenize_str();
  fprintf(stdout, "\ttest_tokenize_int_str\n");
  test_tokenize_int_str();
  fprintf(stdout, "\ttest_tokenize_list\n");
  test_tokenize_list();
  fprintf(stdout, "\ttest_tokenize_nested_list\n");
  test_tokenize_nested_list();
  fprintf(stdout, "\ttest_tokenize_dict\n");
  test_tokenize_dict();
  fprintf(stdout, "\ttest_tokenize_str_with_null_char\n");
  test_tokenize_str_with_null_char();

  fprintf(stdout, "\ttest_read_file\n");
  test_read_file();

  fprintf(stdout, "\ttest_sample_torrent\n");
  test_sample_torrent();
  fprintf(stdout, "\ttest_debian_torrent\n");
  test_debian_torrent();

  fprintf(stdout, "\ttest_init_peer\n");
  test_init_peer();

  fprintf(stdout, "\ttest_contact_tracker\n");
  test_contact_tracker();

  fprintf(stdout, "\ttest_perform_handshake\n");
  test_perform_handshake();

  // fprintf(stdout, "\ttest_read_message\n");
  // test_read_message();

  // fprintf(stdout, "\ttest_bitfield\n");
  // test_bitfield();

  // fprintf(stdout, "\ttest_pieces_queue\n");
  // test_pieces_queue();

  fprintf(stdout, "done\n");
}

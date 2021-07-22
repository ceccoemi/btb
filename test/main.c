#include <stdio.h>

#include "big_endian_test.h"
#include "bitfield_test.h"
#include "client_test.h"
#include "conn_test.h"
#include "file_buf_test.h"
#include "handshake_msg_test.h"
#include "message_test.h"
#include "peer_test.h"
#include "pieces_pool_test.h"
#include "tokenizer_test.h"
#include "torrent_file_test.h"

int main(void)
{
  fprintf(stdout, "Running tests...\n");

  /*
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

    fprintf(stdout, "\ttest_lu_to_big_endian_4_bytes\n");
    test_lu_to_big_endian_4_bytes();
    fprintf(stdout, "\ttest_lu_to_big_endian_2_bytes\n");
    test_lu_to_big_endian_2_bytes();
    fprintf(stdout, "\ttest_lu_to_big_endian_1_bytes\n");
    test_lu_to_big_endian_1_bytes();
    fprintf(stdout, "\ttest_big_endian_to_lu\n");
    test_big_endian_to_lu();
    fprintf(stdout, "\ttest_big_endian_to_lu_with_cast\n");
    test_big_endian_to_lu_with_cast();

    fprintf(stdout, "\ttest_handshake_msg\n");
    test_handshake_msg();
    fprintf(stdout, "\ttest_encoding_decoding_handshake_msg\n");
    test_encoding_decoding_handshake_msg();

    fprintf(stdout, "\ttest_sample_torrent\n");
    test_sample_torrent();
    fprintf(stdout, "\ttest_debian_torrent\n");
    test_debian_torrent();

    fprintf(stdout, "\ttest_peer\n");
    test_peer();

    fprintf(stdout, "\ttest_message\n");
    test_message();
    fprintf(stdout, "\ttest_message_encode_decode\n");
    test_message_encode_decode();

    fprintf(stdout, "\ttest_bitfield\n");
    test_bitfield();

    fprintf(stdout, "\ttest_pieces_pool\n");
    test_pieces_pool();

  fprintf(stdout, "\ttest_init_conn\n");
  test_init_conn();
  fprintf(stdout, "\ttest_init_conn_fail\n");
  test_init_conn_fail();
  fprintf(stdout, "\ttest_send_data\n");
  test_send_data();
  fprintf(stdout, "\ttest_receive_data_with_timeout\n");
  test_receive_data_with_timeout();
    */

  fprintf(stdout, "\ttest_client\n");
  test_client();

  fprintf(stdout, "done\n");
}

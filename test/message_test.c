#include "message_test.h"

#include <stdio.h>
#include <string.h>

#include "../src/defer.h"
#include "../src/message.h"

void test_message()
{
  uint8_t want_msg_id = MSG_ID_CHOKE;
  size_t want_payload_len = 2;
  char want_payload[want_payload_len];
  want_payload[0] = 'a';
  want_payload[1] = 'b';
  message *msg = init_message(want_msg_id, want_payload_len, want_payload);
  DEFER({ free_message(msg); });
  if (msg == NULL) {
    fprintf(stderr, "init_message returns NULL\n");
  }
  if (msg->id != want_msg_id) {
    fprintf(stderr, "wrong message ID: got %d, want %d\n", msg->id, want_msg_id);
  }
  if (msg->payload_len != want_payload_len) {
    fprintf(stderr, "wrong message payload_len: got %lu, want %lu\n", msg->payload_len,
            want_payload_len);
  }
  if (memcmp(msg->payload, want_payload, msg->payload_len) != 0) {
    fprintf(stderr, "wrong message payload: got %*.s, want %*.s\n", (int)msg->payload_len,
            msg->payload, (int)want_payload_len, want_payload);
  }
}

void test_message_encode_decode()
{
  uint8_t want_msg_id = MSG_ID_CHOKE;

  size_t want_payload_len = 9;
  char want_payload[want_payload_len];
  for (size_t i = 0; i < want_payload_len; i++) {
    want_payload[i] = 'x';
  }

  message *msg = init_message(want_msg_id, want_payload_len, want_payload);
  DEFER({ free_message(msg); });
  if (msg == NULL) {
    fprintf(stderr, "init_message returned NULL\n");
  }

  message_encoded *encoded = encode_message(msg);
  DEFER({ free_message_encoded(encoded); });
  if (encoded == NULL) {
    fprintf(stderr, "encode_message returned NULL\n");
    return;
  }
  fprintf(stdout, "MSG LENGTH: %d %d %d %d\n", encoded->buf[0], encoded->buf[1], encoded->buf[2],
          encoded->buf[3]);
  if (encoded->size < 1) {
    fprintf(stderr, "message_encoded should have at least size 1\n");
    return;
  }
  message *decoded = decode_message(encoded->buf, encoded->size);
  DEFER({ free_message(decoded); });
  if (decoded == NULL) {
    fprintf(stderr, "decode_message returned NULL\n");
    return;
  }
  if (memcmp(decoded->payload, want_payload, decoded->payload_len) != 0) {
    fprintf(stderr, "wrong payload in decoded message: got %*.s, want %*.s\n",
            (int)decoded->payload_len, decoded->payload, (int)want_payload_len, want_payload);
    return;
  }
}
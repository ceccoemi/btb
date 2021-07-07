#include "message_test.h"

#include <stdio.h>
#include <string.h>

#include "../src/message.h"

void test_message()
{
  uint8_t want_msg_id = MSG_ID_CHOKE;
  size_t want_payload_len = 2;
  char *want_payload = malloc(want_payload_len);
  want_payload[0] = 'a';
  want_payload[1] = 'b';
  message *msg = init_message(want_msg_id, want_payload_len, want_payload);
  if (msg == NULL) {
    fprintf(stderr, "init_message returns NULL\n");
    goto exit;
  }
  if (msg->id != want_msg_id) {
    fprintf(stderr, "wrong message ID: got %d, want %d\n", msg->id, want_msg_id);
    goto exit;
  }
  if (msg->payload_len != want_payload_len) {
    fprintf(stderr, "wrong message payload_len: got %lu, want %lu\n", msg->payload_len,
            want_payload_len);
    goto exit;
  }
  if (memcmp(msg->payload, want_payload, msg->payload_len) != 0) {
    fprintf(stderr, "wrong message payload: got %*.s, want %*.s\n", (int)msg->payload_len,
            msg->payload, (int)want_payload_len, want_payload);
    goto exit;
  }

exit:
  free(want_payload);
  free_message(msg);
}
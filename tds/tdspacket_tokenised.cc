#include "../tdspsql.h"

tdspacket_tokenised::tdspacket_tokenised(uint32_t tdsversion) : tdspacket(tdsversion) {
//  packet_type = TDS_MESSAGE_TYPE_RESPONSE;
//  packet_status.eom_flag = 1;
  token_head = NULL;
  token_tail = NULL;
}

int
tdspacket_tokenised::writepacket(dynbuffer *buffer) {
  tdstoken *token;
  dynbuffer *child_buffer;
  int done_flag = 0;

  //buffer->add_u8(TDS_MESSAGE_TYPE_RESPONSE);
  //tdspacket::writepacket(buffer);

  for (token = token_head; token; token = token->next) {
    child_buffer = new dynbuffer(buffer);
    if (token->get_type() == 0xFD) {
      if (token->next) {
        tdstoken_done *ttd = (tdstoken_done *)token;
        ttd->set_status_more(1);
      } else {
        done_flag = 1;
      }
    }
    token->writetoken(child_buffer, tdsversion);
    child_buffer->finalise();
    delete child_buffer;
  }
  if (!done_flag) {
    /* append a done token if not already a last one */
    // TODO - check for error packets above and set appropriate flag
    printf("appending done token\n");
    child_buffer = new dynbuffer(buffer);
    tdstoken_done *ttd = new tdstoken_done();
    ttd->writetoken(child_buffer, tdsversion);
    child_buffer->finalise();
    delete child_buffer;
  }
  return 1;
}

void
tdspacket_tokenised::add_token(tdstoken *token) {
  if (!token_head) {
    token_head = token;
    token_tail = token;
    return;
  }
  token_tail->next = token;
  token_tail = token;
}


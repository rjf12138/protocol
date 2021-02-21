#ifndef __PROTOCOL__
#define __PROTOCOL__

#include "basic_head.h"
#include "msg_record.h"
#include "buffer/byte_buffer.h"
#include "http/http.h"
#include "websocket/websocket.h"

namespace my_utils {
    int encode_base64(char *inbuf, int inlen, char *outbuf, int outlen);
    int decode_base64(char* inbuf, int inlen, char* outbuf, int outlen);
};

#endif
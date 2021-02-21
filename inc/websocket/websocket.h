#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include "basic_head.h"

typedef enum ENUM_WEBSOCKET_OPCODE {
    WEBSOCKET_OPCODE_UNKNOWN = -1,
    WEBSOCKET_OPCODE_CONTINUATION_FRAME = 0x01,
    WEBSOCKET_OPCODE_TEXT_FRAME = 0x02,
    WEBSOCKET_OPCODE_BINARY_FRAME = 0x03,
    WEBSOCKET_OPCODE_RESERVED_FOR_NON_CONTROL_FRAMES,
    WEBSOCKET_OPCODE_CONNECTION_CLOSE = 0x08,
    WEBSOCKET_OPCODE_PING = 0x09,
    WEBSOCKET_OPCODE_PONG = 0x0A,
    WEBSOCKET_OPCODE_RESERVED_FOR_CONTROL_FRAMES
}ENUM_WEBSOCKET_OPCODE;

class WebsocketPtl {
public:
    WebsocketPtl(void);
    virtual ~WebsocketPtl(void);

    int parse(const char* data, int nLen);
    int create_packet(const char* buf, int nLen, char* outbuf, int nOutlen, int8_t nOpcode, bool bMask = true);
    string get_content(void);
    int get_op_code(void);

    uint64_t ntohll(uint64_t val);
    uint64_t htonll(uint64_t val);
    int check_end(void);
    int print_hex(int8_t val);
 
private:
    int _msg_pos;
    int _fin;
    int _opcode;
    int _mask;
    int _payload_length;
    char _masking_key[4];
    string _szData;
};

#endif
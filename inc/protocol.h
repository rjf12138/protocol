#ifndef __PROTOCOL__
#define __PROTOCOL__

#include "basic_head.h"
#include "msg_record.h"
#include "buffer/byte_buffer.h"

enum ProtocolType {
    ProtocolType_Raw = 0,           // 原始数据，不做任何解析
    ProtocolType_Http = 1,          // HTTP 协议
    ProtocolType_Websocket = 2      // websocket 协议
};

enum ProtocolParseState {
    ProtocolParseState_Wait = 0,       // 还没开始解析，等待收到数据
    ProtocolType_Parsing = 1,          // 当前解析一半，等待剩余数据
};

namespace my_utils {
int encode_base64(char *inbuf, int inlen, char *outbuf, int outlen);
int decode_base64(char* inbuf, int inlen, char* outbuf, int outlen);

class Protocol {
public:
    Protocol(void);
    Protocol(ProtocolType type);
    virtual ~Protocol(void);

    void set_protocol_type(ProtocolType type);
    ProtocolType get_protocol_type(void) const;

    virtual ByteBuffer& get_protocol_buff(void);

private:
    ProtocolType protocol_type_;
    ProtocolParseState parse_state_;
};

};

#endif
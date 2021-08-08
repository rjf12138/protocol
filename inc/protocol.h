#ifndef __PROTOCOL__
#define __PROTOCOL__

#include "http.h"
#include "websocket.h"

namespace protocol {
    
enum ProtocolType {
    ProtocolType_Raw = 0,           // 原始数据，不做任何解析
    ProtocolType_Http = 1,          // HTTP 协议
    ProtocolType_Websocket = 2      // websocket 协议
};

enum ProtocolParseState {
    ProtocolParseState_Wait = 0,       // 还没开始解析，等待收到数据
    ProtocolType_Parsing = 1,          // 当前解析一半，等待剩余数据
};

class ProtocolParser {
public:
    ProtocolParser(void);
    ProtocolParser(ProtocolType type);
    virtual ~ProtocolParser(void);

    void clear(void);
    void set_protocol_type(ProtocolType type);
    ProtocolType get_protocol_type(void) const;
    basic::ByteBuffer& get_protocol_buff(void);

    HttpParse_ErrorCode get_http_packet(HttpPtl &ptl);
    WebsocketParse_ErrorCode get_websocket_packet(WebsocketPtl &ptl);
    
private:
    ProtocolType protocol_type_;
    ProtocolParseState parse_state_;
    basic::ByteBuffer data_;
};

};

#endif
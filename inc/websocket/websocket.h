#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include "basic_head.h"
#include "buffer/byte_buffer.h"

namespace my_protocol {
    
using namespace my_utils;

enum WebsocketParse_ErrorCode {
    WebsocketParse_OK = 0,
    WebsocketParse_PacketNotEnough = -1,
    WebsocketParse_NotSupportWebsocket = -2
};

enum ENUM_WEBSOCKET_OPCODE {
    WEBSOCKET_OPCODE_UNKNOWN = -1,
    WEBSOCKET_OPCODE_CONTINUATION_FRAME = 0x01,
    WEBSOCKET_OPCODE_TEXT_FRAME = 0x02,
    WEBSOCKET_OPCODE_BINARY_FRAME = 0x03,
    WEBSOCKET_OPCODE_RESERVED_FOR_NON_CONTROL_FRAMES,
    WEBSOCKET_OPCODE_CONNECTION_CLOSE = 0x08,
    WEBSOCKET_OPCODE_PING = 0x09,
    WEBSOCKET_OPCODE_PONG = 0x0A,
    WEBSOCKET_OPCODE_RESERVED_FOR_CONTROL_FRAMES
};

class WebsocketPtl {
public:
    WebsocketPtl(void);
    virtual ~WebsocketPtl(void);

    WebsocketParse_ErrorCode parse(ByteBuffer &buff);
    Int32 generate(ByteBuffer &out, ByteBuffer &content, Int8 nOpcode, bool bMask = false);

    // 获取消息内容
    Int32 get_content(ByteBuffer &out);
    // 获取opcode
    ENUM_WEBSOCKET_OPCODE get_op_code(void);

    // 64位整型大小端转换
    UInt64 ntohll(UInt64 val);
    UInt64 htonll(UInt64 val);

    // 检查大小端
    Int32 check_end(void);
    // 打印成16进制
    Int32 print_hex(Int8 val);
 
private:
    Int8 fin_;
    ENUM_WEBSOCKET_OPCODE opcode_;

    ByteBuffer data_;
};

}
#endif
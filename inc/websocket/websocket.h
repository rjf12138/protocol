#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include "basic_head.h"
#include "http/http.h"

namespace my_protocol {
    
using namespace my_utils;

enum WebsocketParse_ErrorCode {
    WebsocketParse_OK = 0,
    WebsocketParse_PacketNotEnough = -1,
    WebsocketParse_ParseFailed = -2,
    WebsocketParse_NotSupportWebsocket = -3
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

    // 客户端请求将链接升级为 websocket 的 http 请求包
    int get_upgrade_packet(HttpPtl &request, ByteBuffer &content, string url = "/", string host = "websocket client");
    // 服务端回复客户端的请求
    int response_upgrade_packet(HttpPtl &request, HttpPtl &response, ByteBuffer &content, string host = "websocket server");

    // 获取消息内容
    Int32 get_content(ByteBuffer &out);
    // 获取opcode
    ENUM_WEBSOCKET_OPCODE get_opcode(void);

    // 64位整型大小端转换
    UInt64 ntohll(UInt64 val);
    UInt64 htonll(UInt64 val);

    // 检查大小端
    Int32 check_end(void);
    // 打印成16进制
    Int32 print_hex(Int8 val);

private:
    int generate_sec_websocket_key(ByteBuffer &out);
    int generate_sec_websocket_accept(ByteBuffer &sec_key);
private:
    Int8 fin_;
    ByteBuffer sec_websocket_accept_;
    ENUM_WEBSOCKET_OPCODE opcode_;

    ByteBuffer data_;
};

}
#endif
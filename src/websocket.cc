#include "algorithm/algorithm.h"
#include "protocol.h"

namespace ptl {

WebsocketPtl::WebsocketPtl(void)
{
}

WebsocketPtl::~WebsocketPtl(void)
{
}

int32_t 
WebsocketPtl::print_hex(int8_t val)
{
    const char *buf = "0123456789ABCDEF";
    int32_t high = val >> 4 & 0x0f;
    int32_t low = val & 0x0f;

    printf("%c%c", buf[high], buf[low]);

    return 0;
}

void 
WebsocketPtl::clear(void)
{
    fin_ = 0;
    sec_websocket_accept_.clear();
    data_.clear();
    opcode_ = WEBSOCKET_OPCODE_UNKNOWN;
}

uint64_t 
WebsocketPtl::ntohll(uint64_t host)
{
    if (check_end() == 0)
    {
        return host;
    }
    uint64_t   ret = 0;
    uint32_t   high, low;

    low = host & 0xFFFFFFFF;
    high = (host >> 32) & 0xFFFFFFFF;
    low = ntohl(low);
    high = ntohl(high);

    ret = low;
    ret <<= 32;
    ret |= high;

    return ret;
}

uint64_t 
WebsocketPtl::htonll(uint64_t host)
{
    if (check_end() == 0)
    {
        return host;
    }
    uint64_t   ret = 0;
    uint32_t   high, low;

    low = host & 0xFFFFFFFF;
    high = (host >> 32) & 0xFFFFFFFF;
    low = htonl(low);
    high = htonl(high);

    ret = low;
    ret <<= 32;
    ret |= high;

    return ret;
}

int32_t
WebsocketPtl::check_end()
{
    union {
        char c;
        int32_t i;
    }un;
    un.i = 1;
    return un.c;
}

int 
WebsocketPtl::generate_sec_websocket_key(basic::ByteBuffer &out)
{
    basic::ByteBuffer buffer;
    for (int i = 0; i < 16; ++i) {
        buffer.write_int8(rand() % 256);
    }
    out.clear();
    algorithm::encode_base64(buffer, out);  // 生成发送的sec-key

    buffer = out;
    this->generate_sec_websocket_accept(buffer);

    return 0;
}

int 
WebsocketPtl::generate_sec_websocket_accept(basic::ByteBuffer &sec_key)
{
    // 生成 accept-key 用于检查服务端返回
    sec_websocket_accept_.clear();
    sec_key.write_string("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    algorithm::sha1(sec_key, sec_websocket_accept_);

    return 0;
}

WebsocketParse_ErrorCode 
WebsocketPtl::parse(basic::ByteBuffer &buff)
{
    uint8_t mask = 0;
    uint8_t masking_key[4] = {0};
    uint64_t msg_pos = 0;
    uint64_t payload_length = 0;
    uint64_t len = buff.data_size();
    if (len >= 1) {
        fin_ = (unsigned char)buff[msg_pos] >> 7;
        if ((buff[msg_pos] & 0x70) != 0x0) { // 该实现不支持扩展，所以RSV1, RSV2, RSV3必须是0，否则关闭返回错误关闭连接
            return WebsocketParse_ParseFailed;
        }
        opcode_ = static_cast<ENUM_WEBSOCKET_OPCODE>(buff[msg_pos] & 0x0f);
        ++msg_pos;
    } else {
        return WebsocketParse_PacketNotEnough;
    }

    if (len >= 2) {
        mask = (uint8_t)buff[msg_pos] >> 7;
        payload_length = buff[msg_pos] & 0x7F;
        msg_pos++;
    } else {
        return WebsocketParse_PacketNotEnough;
    }
       
    if (payload_length < 126 && payload_length >= 0) {
        // 
    } else if (payload_length == 126 && len >= msg_pos + 2) {
        uint16_t length = 0;
        buff.read_only(msg_pos, &length, sizeof(uint16_t));
        msg_pos += 2;
        payload_length = ::ntohs(length);
    } else if (payload_length == 127 && len >= msg_pos + 8) {
        uint64_t length = 0;
        buff.read_only(msg_pos, &length, sizeof(uint64_t));
        msg_pos += 8;
        payload_length = this->ntohll(length);
    } else {
        return WebsocketParse_PacketNotEnough;
    }
    
    if (mask == 1 && len >= msg_pos + 4) {
        for (int32_t i = 0; i < 4; ++i) {
            masking_key[i] = buff[msg_pos + i];
        }
        msg_pos += 4;
    } else if (mask == 0) {
    } else {
        return WebsocketParse_PacketNotEnough;
    }
    
    if (len >= msg_pos + payload_length) {
        data_.clear();
        if (mask != 1) {
            auto copy_start_iter = buff.begin() + msg_pos;
            buff.get_data(data_, copy_start_iter, payload_length);
        } else {
            for (uint64_t i = 0; i < payload_length; ++i) {
                data_.write_int8(buff[msg_pos + i] ^ masking_key[i % 4]);
            }
        }
        msg_pos += payload_length;
    } else {
        return WebsocketParse_PacketNotEnough;
    }
    buff.update_read_pos(msg_pos);

    return WebsocketParse_OK;
}

int 
WebsocketPtl::get_upgrade_packet(HttpPtl &request, basic::ByteBuffer &content, std::string url, std::string host)
{
    request.clear();
    request.set_request(HTTP_METHOD_GET, url);
    request.set_header_option(HTTP_HEADER_Host, host);
    request.set_header_option(HTTP_HEADER_Upgrade, "websocket");
    request.set_header_option(HTTP_HEADER_Connection, "Upgrade");
    request.set_header_option(HTTP_HEADER_SecWebSocketVersion, "13");

    basic::ByteBuffer buffer;
    this->generate_sec_websocket_key(buffer);
    request.set_header_option(HTTP_HEADER_SecWebSocketKey, buffer.str());
    request.set_content(content);

    return 0;
}

int 
WebsocketPtl::response_upgrade_packet(HttpPtl &request, HttpPtl &response, basic::ByteBuffer &content, std::string host)
{
    if (request.get_method() != HTTP_METHOD_GET) {
        return -1;
    }

    if (request.get_header_option(HTTP_HEADER_Upgrade) == "websocket") {
        return -1;
    }

    if (request.get_header_option(HTTP_HEADER_Connection) == "Upgrade") {
        return -1;
    }

    if (request.get_header_option(HTTP_HEADER_SecWebSocketVersion) == "13") {
        return -1;
    }

    basic::ByteBuffer buffer;
    buffer.write_string(request.get_header_option(HTTP_HEADER_SecWebSocketKey));
    this->generate_sec_websocket_accept(buffer);

    response.clear();
    response.set_response(101, "Switching Protocols");
    response.set_header_option(HTTP_HEADER_Connection, "Upgrade");
    response.set_header_option(HTTP_HEADER_SecWebSocketVersion, "13");
    response.set_header_option(HTTP_HEADER_SecWebSocketAccept, sec_websocket_accept_.str());
    response.set_content(content);

    return 0;
}

int32_t 
WebsocketPtl::generate(basic::ByteBuffer &out, basic::ByteBuffer &content, int8_t nOpcode, bool bMask)
{
    out.clear();

    ssize_t len = content.data_size();
    int32_t start_pos = 0;
    // add fin and opcode
    char cbyte = 0x8;
    cbyte = (cbyte << 4) | (nOpcode & 0xf);
    out.write_int8(cbyte);
    start_pos++;

    // add mask msg length
    cbyte = 0;
    if (bMask) {
        cbyte = 0x8 << 4;
    }

    if (len >= 0 && len < 126) {
        cbyte |= len & 0x7F;
        out.write_int8(cbyte);
        start_pos++;
    } else if (len >= 126 && len < 0xFFFF) {
        cbyte |= 126;
        out.write_int8(cbyte);
        start_pos++;
        out.write_int16(htons(len));
        start_pos += 2;
    } else if (len >= 0xFFFF) {
        cbyte |= 127;
        out.write_int8(cbyte);
        start_pos++;
        uint64_t uLength = this->htonll(len & 0x7FFFFFFFFFFFFFFF);
        out.write_int64(uLength);
        start_pos += 8;
    }
    
    if (bMask == true) {
        int32_t mask_num = static_cast<int32_t>(time(NULL));

        out.write_int32(mask_num);
        char *mask_ch = (char*)&mask_num; // 直接对mask_num取指针用下标访问有问题

        for (unsigned i = 0; i < len; ++i) {
            out.write_int8(content[i] ^ mask_ch[i % 4]);
        }
    } else {
        out += content;
    }

    return out.data_size();
}

basic::ByteBuffer &
WebsocketPtl::get_content(void)
{   
    return data_;
}

ENUM_WEBSOCKET_OPCODE 
WebsocketPtl::get_opcode(void)
{
    if (fin_ == 1) { // 只有在该帧为末尾时才返回
        return opcode_;
    }

    return WEBSOCKET_OPCODE_UNKNOWN;
}

}
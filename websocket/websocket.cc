#include "websocket.h"

namespace my_protocol {

WebsocketPtl::WebsocketPtl(void)
{
}

WebsocketPtl::~WebsocketPtl(void)
{
}

Int32 
WebsocketPtl::print_hex(Int8 val)
{
    const char *buf = "0123456789ABCDEF";
    Int32 high = val >> 4 & 0x0f;
    Int32 low = val & 0x0f;

    printf("%c%c", buf[high], buf[low]);

    return 0;
}

UInt64 
WebsocketPtl::ntohll(UInt64 host)
{
    if (check_end() == 0)
    {
        return host;
    }
    UInt64   ret = 0;
    UInt32   high, low;

    low = host & 0xFFFFFFFF;
    high = (host >> 32) & 0xFFFFFFFF;
    low = ntohl(low);
    high = ntohl(high);

    ret = low;
    ret <<= 32;
    ret |= high;

    return ret;
}

UInt64 
WebsocketPtl::htonll(UInt64 host)
{
    if (check_end() == 0)
    {
        return host;
    }
    UInt64   ret = 0;
    UInt32   high, low;

    low = host & 0xFFFFFFFF;
    high = (host >> 32) & 0xFFFFFFFF;
    low = htonl(low);
    high = htonl(high);

    ret = low;
    ret <<= 32;
    ret |= high;

    return ret;
}

Int32
WebsocketPtl::check_end()
{
    union {
        char c;
        Int32 i;
    }un;
    un.i = 1;
    return un.c;
}

WebsocketParse_ErrorCode 
WebsocketPtl::parse(ByteBuffer &buff)
{
    UInt8 mask = 0;
    UInt8 masking_key[4] = {0};
    UInt64 msg_pos = 0;
    UInt64 payload_length = 0;
    UInt64 len = buff.data_size();
    if (len >= 1) {
        fin_ = (unsigned char)buff[msg_pos] >> 7;
        opcode_ = static_cast<ENUM_WEBSOCKET_OPCODE>(buff[msg_pos] & 0x0f);
        msg_pos++;
    } else {
        return WebsocketParse_PacketNotEnough;
    }

    if (len >= 2) {
        mask = (UInt8)buff[msg_pos] >> 7;
        payload_length = buff[msg_pos] & 0x7f;
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
        UInt64 length = 0;
        buff.read_only(msg_pos, &length, sizeof(UInt64));
        msg_pos += 8;
        payload_length = this->ntohll(length);
    } else {
        return WebsocketParse_PacketNotEnough;
    }

    if (mask == 1 && len >= msg_pos + 4) {
        for (Int32 i = 0; i < 4; i++) {
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
            for (UInt64 i = 0; i < payload_length; i++) {
                data_.write_int8(buff[msg_pos + i] ^ masking_key[i % 4]);
            }
        }
        msg_pos += payload_length;
    } else {
        return WebsocketParse_PacketNotEnough;
    }
    return WebsocketParse_OK;
}

Int32 
WebsocketPtl::generate(ByteBuffer &out, ByteBuffer &content, Int8 nOpcode, bool bMask)
{
    out.clear();

    BUFSIZE_T len = content.data_size();
    Int32 start_pos = 0;
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
        cbyte |= len & 0x7;
        out.write_int8(cbyte);
        start_pos++;
    } else if (len >= 126 && len <= 0xFFFF) {
        cbyte |= 126;
        out.write_int8(cbyte);
        start_pos++;
        out.write_int16_hton(len);
        start_pos += 2;
    } else if (len >= 0xFFFF) {
        cbyte |= 127;
        out.write_int8(cbyte);
        start_pos++;
        UInt64 uLength = this->htonll(len) & 0x7FFFFFFFFFFFFFFF;
        out.write_int64(uLength);
        start_pos += 8;
    }

    if (bMask) {
        srand(time(NULL));
        UInt32 rand_num = rand();
        out.write_int32(rand_num);

        for (unsigned i = 0; i < len; i++) {
            out.write_int8(content[i] ^ (&rand_num)[i % 4]);
        }
    } else {
        out += content;
    }
    
    return out.data_size();
}

Int32
WebsocketPtl::get_content(ByteBuffer &out)
{
    if (fin_ == 1)
    {
        out = data_;
        return out.data_size();
    }
    
    return 0;
}

ENUM_WEBSOCKET_OPCODE 
WebsocketPtl::get_op_code(void)
{
    if (fin_ == 1) { // 只有在该帧为末尾时才返回
        return opcode_;
    }

    return WEBSOCKET_OPCODE_UNKNOWN;
}

}
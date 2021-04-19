#include "websocket.h"

namespace my_protocol {

WebsocketPtl::WebsocketPtl(void)
{
}

WebsocketPtl::~WebsocketPtl(void)
{
}

int 
WebsocketPtl::print_hex(int8_t val)
{
    const char *buf = "0123456789ABCDEF";
    int high = val >> 4 & 0x0f;
    int low = val & 0x0f;

    printf("%c%c", buf[high], buf[low]);

    return 0;
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

int
WebsocketPtl::check_end()
{
    union
    {
        char c;
        int i;
    }un;
    un.i = 1;
    return un.c;
}

WebsocketParse_ErrorCode 
WebsocketPtl::parse(ByteBuffer &buff)
{
    int len = buff.data_size();
    msg_pos_ = 0;
    if (len >= 1)
    {
        fin_ = (unsigned char)buff[msg_pos_] >> 7;
        opcode_ = static_cast<ENUM_WEBSOCKET_OPCODE>(buff[msg_pos_] & 0x0f);
        msg_pos_++;
    }
    else
    {
        return WebsocketParse_PacketNotEnough;
    }

    if (len >= 2)
    {
        mask_ = (unsigned char)buff[msg_pos_] >> 7;
        payload_length_ = buff[msg_pos_] & 0x7f;
        msg_pos_++;
    }
    else
    {
        return WebsocketParse_PacketNotEnough;
    }
       
    if (payload_length_ < 126 && payload_length_ >= 0)
    {
        // 
    }
    else if (payload_length_ == 126 && len >= msg_pos_ + 2) 
    {
        uint16_t length = 0;
        
        memcpy(&length, data + msg_pos_, 2);
        msg_pos_ += 2;
        payload_length_ = ::ntohs(length);
    }
    else if (payload_length_ == 127 && nLen >= msg_pos_ + 8) 
    {
        uint64_t length = 0;
        memcpy(&length, data + msg_pos_, 8);
        msg_pos_ += 8;
        payload_length_ = this->ntohll(length);
    }
    else
    {
        return EHPR_NULL;
    }

    if (mask_ == 1 && nLen >= msg_pos_ +4)
    {
        for (int i = 0; i < 4; i++)
        {
            _masking_key[i] = data[msg_pos_ + i];
        }
        msg_pos_ += 4;
    }
    else if (mask_ == 0)
    {
    }
    else
    {
        return EHPR_NULL;
    }

    if (nLen >= msg_pos_ + payload_length_)
    {
        char* szPayload = new char[payload_length_];
        memset(szPayload, 0, payload_length_);
        if (mask_ != 1)
        {
            memcpy(szPayload, data + msg_pos_, payload_length_);
        }
        else
        {
            for (unsigned i = 0; i < payload_length_; i++)
            {
                int j = i % 4;
                szPayload[i] = data[msg_pos_ + i] ^ _masking_key[j];
            }
        }
        msg_pos_ += payload_length_;
        _szData = szPayload;
    }
    else
    {
        return EHPR_NULL;
    }
    return EHPR_COMPELETE;
}

int 
WebsocketPtl::generate(ByteBuffer &out, ByteBuffer &content, int8_t nOpcode, bool bMask)
{
    out.clear();

    BUFSIZE_T len = content.data_size();
    int start_pos = 0;
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
        uint64_t uLength = this->htonll(len) & 0x7FFFFFFFFFFFFFFF;
        out.write_int64(uLength);
        start_pos += 8;
    }

    if (bMask) {
        srand(time(NULL));
        uint32_t rand_num = rand();
        out.write_int32(rand_num);

        for (unsigned i = 0; i < len; i++) {
            out.write_int8(content[i] ^ (&rand_num)[i % 4]);
        }
    } else {
        out += content;
    }
    
    return out.data_size();
}

int
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
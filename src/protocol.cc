#include "protocol.h"

namespace my_utils {

ProtocolParser::ProtocolParser(void)
    : protocol_type_(ProtocolType_Raw),
      parse_state_(ProtocolParseState_Wait)
{
}

ProtocolParser::ProtocolParser(ProtocolType type)
    : protocol_type_(type),
      parse_state_(ProtocolParseState_Wait)
{

}

ProtocolParser::~ProtocolParser(void)
{

}

void 
ProtocolParser::set_protocol_type(ProtocolType type)
{
    protocol_type_ = type;
}

ProtocolType 
ProtocolParser::get_protocol_type(void) const
{
    return protocol_type_;
}

ByteBuffer& 
ProtocolParser::get_protocol_buff(void)
{
    return data_;
}

int 
ProtocolParser::get_http_packet(HttpPtl &ptl)
{
    return ptl.parser(data_);
}

};
#include "protocol.h"

namespace my_protocol {

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
ProtocolParser::clear(void)
{
    data_.clear();
    protocol_type_ = ProtocolType_Raw;
    parse_state_ = ProtocolParseState_Wait;
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

HttpParse_ErrorCode 
ProtocolParser::get_http_packet(HttpPtl &ptl)
{
    if (protocol_type_ != ProtocolType_Http) {
        return HttpParse_NotSupportHttp;
    }
    return ptl.parser(data_);
}

WebsocketParse_ErrorCode 
ProtocolParser::get_websocket_packet(WebsocketPtl &ptl)
{
    if (protocol_type_ != ProtocolType_Websocket) {
        return WebsocketParse_NotSupportWebsocket;
    }

    return ptl.parse(data_);
}

};
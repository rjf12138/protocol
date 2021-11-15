#include "protocol.h"

namespace ptl {
enum ParserState {
    ParserState_Protocol,
    ParserState_Addr,
    ParserState_Port,
    ParserState_ResPath,
    ParserState_Param,
    ParserState_Complete
};

enum ParserParam {
    ParserParam_Key,
    ParserParam_Value,
    ParserParam_End,
};

URLParser::URLParser(void)
{

}


URLParser::~URLParser(void)
{

}

void 
URLParser::clear(void)
{
    type_ = ptl::ProtocolType_Tcp;
    addr_.clear();
    port_ = 0;
    res_path_.clear();
    param_.clear();
}

ParserError 
URLParser::parser(const std::string &url)
{
    this->clear();
    ParserState state = ParserState_Protocol;
    for (std::size_t i = 0; i < url.length();) {
        switch (state)
        {
        case ParserState_Protocol: {
            if (url[i] == 'h' || url[i] == 'H') {
                if (url.length() > 7 && strncasecmp(url.c_str(), "http://", 7) == 0) {
                    type_ = ptl::ProtocolType_Http;
                    state = ParserState_Addr;
                    i += 7;
                } else {
                    return url.length() <= 7 ? ParserError_IncompleteURL:ParserError_UnknownPtl;
                }
            } else if (url[i] == 'w' || url[i] == 'W') {
                if (url.length() > 5 && strncasecmp(url.c_str(), "ws://", 5) == 0) {
                    type_ = ptl::ProtocolType_Websocket;
                    state = ParserState_Addr;
                    i += 5;
                } else {
                    return url.length() <= 5 ? ParserError_IncompleteURL:ParserError_UnknownPtl;
                }
            } else if (url[i] == 't' || url[i] == 'T') {
                if (url.length() > 6 && strncasecmp(url.c_str(), "tcp://", 6) == 0) {
                    type_ = ptl::ProtocolType_Tcp;
                    state = ParserState_Addr;
                    i += 6;
                } else {
                    return url.length() <= 6 ? ParserError_IncompleteURL:ParserError_UnknownPtl;
                }
            } else {
                return ParserError_UnknownPtl;
            }
        
        } break;
        case ParserState_Addr: {
            for (;i < url.length(); ++i) {
                if (url[i] == ':') {
                    state = ParserState_Port;
                    ++i;
                    break;
                } else if (url[i] == '/') {
                    if (type_ == ptl::ProtocolType_Websocket || type_ == ptl::ProtocolType_Http) {
                        port_ = 80;
                        state = ParserState_ResPath;
                        break;
                    } else {
                        return ParserError_AmbiguousPort;
                    }
                }
                addr_ += url[i];
            }
            if (i == url.length()) {
                if (type_ == ptl::ProtocolType_Websocket || type_ == ptl::ProtocolType_Http) {
                    port_ = port_ == 0 ? 80 : port_;
                    res_path_ = "/";
                    return ParserError_Ok;
                } else {
                    return ParserError_AmbiguousPort;
                }
            }
        } break;
        case ParserState_Port: {
            std::string port;
            for (;i < url.length(); ++i) {
                if (url[i] == '/') {
                    state = ParserState_ResPath;
                    break;
                }

                if (url[i] < '0' || url[i] > '9') {
                    return ParserError_ErrorPort;
                }
                port += url[i];
            }

            port_ = std::atoi(port.c_str());
            if (i == url.length()) {
                if (type_ == ptl::ProtocolType_Websocket || type_ == ptl::ProtocolType_Http) {
                    port_ = (port_ == 0 ? 80 : port_);
                    res_path_ = "/";
                    return ParserError_Ok;
                } else if (type_ == ptl::ProtocolType_Tcp) {
                    if (port_ > 0) {
                        return ParserError_Ok;
                    }
                }
                return ParserError_AmbiguousPort;
            }
        } break;
        case ParserState_ResPath: {
            for (;i < url.length(); ++i) {
                if (url[i] == '?') {
                    state = ParserState_Param;
                    ++i;
                    break;
                }
                res_path_ += url[i];
            }
            if (i == url.length()) {
                return ParserError_Ok;
            }
        } break;
        case ParserState_Param: {
            std::string key, value;
            ParserParam param_state = ParserParam_Key;
            for (;i < url.length(); ++i) {
                switch (param_state) {
                    case ParserParam_Key: {
                        if (url[i] == '=') {
                            param_state = ParserParam_Value;
                            break;
                        }
                        key += url[i];
                    } break;
                    case ParserParam_Value: {
                        if (url[i] == '&') {
                            param_state = ParserParam_Key;
                            param_[key] = value;
                            value = key = "";
                            break;
                        }
                        value += url[i];
                    } break;
                }
            }

            if (param_state == ParserParam_Value) { // 最后一个参数
                param_[key] = value;
                state = ParserState_Complete;
            } else {
                return ParserError_IncompleteParameters;
            }
        } break;
        default:
            break;
        }
    }
    return ParserError_Ok;
}

}
#ifndef __URL_PARSER_H__
#define __URL_PARSER_H__

#include "basic/basic_head.h"

namespace ptl {
// 协议名称：
// raw: raw         // 原始收到的数据不做任何解析
// http: http       // http 协议。 默认端口： 80
// websocket: ws    // websocket 协议。 默认端口： 80

// 格式例子: http://192.168.1.2:80/dir/index.html?uid=1&key=2

enum ParserError {
    ParserError_Ok = 0,                 // 解析正确
    ParserError_UnknownPtl = -1,        // 协议不确定
    ParserError_IncompleteURL = -2,     // url 不完整
    ParserError_AmbiguousPort = -3,     // 端口不明确
    ParserError_ErrorPort = -4,         // 端口错误
};

class URLParser {
public:
    URLParser(void);
    ~URLParser(void);

    // 清除之前保存内容
    void clear(void);
    // 解析url
    ParserError parser(const std::string &url);
    
public:
    ptl::ProtocolType type_;    // 协议类型
    std::string addr_;  // 服务器地址
    int port_;      // 服务器端口
    std::string res_path_; // 资源路径
    std::map<std::string, std::string> param_; // 参数
};
}

#endif
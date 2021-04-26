#ifndef __HTTP_H_
#define __HTTP_H_

#include "basic_head.h"
#include "http_def.h"
#include "buffer/byte_buffer.h"

namespace my_protocol {
    
using namespace my_utils;

enum HttpParse_ErrorCode {
    HttpParse_OK = 0,
    HttpParse_CantFindHttp = -1,
    HttpParse_CantFindBody = -2,
    HttpParse_ContentNotEnough = -3,
    HttpParse_HttpVersionNotMatch = -4,
    HttpParse_ParseHeaderFailed = -5,
    HttpParse_NotSupportHttp = -6
};

class HttpPtl {
public:
    HttpPtl(void);
    virtual ~HttpPtl(void);

    HttpParse_ErrorCode parser(ByteBuffer &data);
    int generate(ByteBuffer &data);

    int clear();

    // 设置http请求头
    int set_request(const string &method, const string &url);
    // 设置http回应头
    int set_response(int code, const string &phrase);
    // 设置回应消息短语
    int set_phrase(string phrase);
    // 设置报文主体内容
    int set_content(ByteBuffer &data);
    // 设置头选项
    int set_header_option(const string &key, const string &value);

    // 获取回应状态码
    int get_status_code(void);
    // 获取http方法
    string get_method(void);
    // 获取消息短语
    string get_phrase(void);
    // 获取消息短语
    string get_header_option(const string &key);
    // 获取报文内容
    BUFSIZE_T get_content(ByteBuffer &data);

private:
    bool is_request;
    int code_;
    string url_;
    string method_;
    string phrase_;
    ByteBuffer content_;
    map<string, string> header_;
};

}
#endif
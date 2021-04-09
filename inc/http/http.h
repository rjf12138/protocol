#ifndef __HTTP_H_
#define __HTTP_H_

#include "basic_head.h"
#include "http_def.h"
#include "buffer/byte_buffer.h"

namespace my_utils {

class HttpPtl {
public:
    HttpPtl(void);
    virtual ~HttpPtl(void);

    int parser(ByteBuffer &data);
    int generate(ByteBuffer &data);

    int set_request(const string &method, const string &url);
    int set_response(int code, const string &phrase);
    int set_content(ByteBuffer &data);
    int set_header_option(const string &key, const string &value);


    int get_status_code(void);
    string get_method(void);
    string get_phrase(void);
    string get_header_option(const string &key);
    int get_content(ByteBuffer &data);
    
private:
    string get_status_phrase(int status);

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
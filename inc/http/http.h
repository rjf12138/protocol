#ifndef __HTTP_H_
#define __HTTP_H_

#include "basic_head.h"
#include "buffer/byte_buffer.h"

namespace my_utils {
#define HTTP_VERSION    "HTTP/1.1"

#define HTTP_METHOD_UNKNOWN "-1"
#define HTTP_METHOD_GET     "GET"
#define HTTP_METHOD_POST    "POST"
#define HTTP_METHOD_PUT     "PUT"
#define HTTP_METHOD_DELETE  "DELETE"
#define HTTP_METHOD_HEAD    "HEAD"
#define HTTP_METHOD_OPTION  "OPTION"

#define HTTP_HEADER_OPTION_

class HttpPtl {
public:
    HttpPtl(void);
    virtual ~HttpPtl(void);

    int parser(ByteBuffer &data);
    int generate(ByteBuffer& data);

    int set_content(ByteBuffer& data);
    int set_header(string method, int status, string reason_phrase);
    int set_header_option(const string &key, const string &value);

    int get_status(void);
    string get_method(void);
    string get_header_option(const string &key);
    int get_content(ByteBuffer &data);

private:
    int status_;
    string method_;
    string reason_phrase_;
    map<string, string> header_;
};

}
#endif
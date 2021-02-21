#ifndef __HTTP_H_
#define __HTTP_H_

#include "basic_head.h"
#include "buffer/byte_buffer.h"
#include "buffer/str_buffer.h"

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

    int parser(const ByteBuffer &data);
    int generate(CRQueue& data);

    int set_content(const CRQueue& data);
    int set_header(string method, int status, string reason_phrase);
    int set_header_option(const string &key, const string &value);

    int get_status(void);
    string get_method(void);
    string get_header_option(const string &key);
    int get_content(CRQueue &data);

private:
    int _status;
    string _method;
    string _reason_phrase;
    map<string, string> _header;
};

#endif
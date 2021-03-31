#ifndef __HTTP_H_
#define __HTTP_H_

#include "basic_head.h"
#include "buffer/byte_buffer.h"

namespace my_utils {

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
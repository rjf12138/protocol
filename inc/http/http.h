#ifndef __HTTP_H_
#define __HTTP_H_

#include "http_def.h"
#include "protocol.h"

namespace my_utils {

class HttpPtl : public Protocol{
public:
    HttpPtl(void);
    HttpPtl(ByteBuffer &data);
    virtual ~HttpPtl(void);

    int parser(ByteBuffer &data);
    int generate(ByteBuffer &data);

    int set_content(ByteBuffer &data);
    int set_header(const string &method, int status);
    int set_header_option(const string &key, const string &value);

    int get_status(void);
    string get_method(void);
    string get_header_option(const string &key);
    int get_content(ByteBuffer &data);
    
private:
    string get_status_phrase(int status);

private:
    int status_;
    string method_;
    ByteBuffer content_;
    map<string, string> header_;
};

}
#endif
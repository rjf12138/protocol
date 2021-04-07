#include "http.h"

namespace my_utils {

HttpPtl::HttpPtl(void)
{

}

HttpPtl::~HttpPtl(void)
{

}

int 
HttpPtl::parser(ByteBuffer &data)
{
    auto iter = data.begin();
    for (; iter != data.end(); ++iter) {
        switch (*iter) {
            case 'G':
            {
                ByteBuffer patten(HTTP_METHOD_GET);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_GET;
                    goto __HTTP_HEAD__;
                }
            } break;
            case 'P':
            {
                ByteBuffer patten(HTTP_METHOD_PUT);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_PUT;
                    goto __HTTP_HEAD__;
                }

                patten.clear();
                patten.write_string(HTTP_METHOD_POST);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_POST;
                    goto __HTTP_HEAD__;
                }
            } break;
            case 'D':
            {
                ByteBuffer patten(HTTP_METHOD_DELETE);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_DELETE;
                    goto __HTTP_HEAD__;
                }
            } break;
            case 'H':
            {
                ByteBuffer patten(HTTP_METHOD_HEAD);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_HEAD;
                    goto __HTTP_HEAD__;
                }

                patten.clear();
                patten.write_string(HTTP_METHOD_RESPONE);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_RESPONE;
                    goto __HTTP_HEAD__;
                }
            } break;
            case 'O':
            {
                ByteBuffer patten(HTTP_METHOD_OPTION);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_OPTION;
                    goto __HTTP_HEAD__;
                }
            } break;
            default:
            {
                continue;
            }
        }
    }

    return -1; // 查找 HTTP 头失败

__HTTP_HEAD__:
    
}

int 
HttpPtl::generate(ByteBuffer &data)
{

}

int 
HttpPtl::set_content(ByteBuffer &data)
{

}

int 
HttpPtl::set_header(const string &method, int status)
{

}

int 
HttpPtl::set_header_option(const string &key, const string &value)
{

}

int 
HttpPtl::get_status(void)
{

}

string 
HttpPtl::get_method(void)
{

}

string 
HttpPtl::get_header_option(const string &key)
{

}

int 
HttpPtl::get_content(ByteBuffer &data)
{

}

string 
HttpPtl::get_status_phrase(int status)
{

}

}
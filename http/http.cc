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
    auto start_pos_iter = data.begin();
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
    data.update_read_pos(iter - start_pos_iter); // 更新ByteBuffer读位置到http协议开始的位置
    ByteBuffer_Iterator http_begin_iter = data.begin();

    ostringstream ostr;
    BUFSIZE_T body_length = 0;

    vector<ByteBuffer_Iterator> http_length_pos = data.find(ByteBuffer(HTTP_HEADER_ContentLength));
    vector<ByteBuffer_Iterator> http_body_pos = data.find(ByteBuffer("\r\n\r\n"));

    if (http_body_pos.size() <= 0 || http_length_pos.size() <= 0) { // 找不到报文主体开始的位置或是找不到报文主体的长度解析失败 
        return -1;
    }

    ByteBuffer_Iterator header_value_start_iter = http_length_pos[0] + strlen(HTTP_HEADER_ContentLength); // 获取报文主体的长度
    for (; header_value_start_iter != data.end(); ++header_value_start_iter) {
        if (*header_value_start_iter == ':' || *header_value_start_iter == ' ') {
            continue;
        } else if (*header_value_start_iter == '\r' && *(header_value_start_iter+1) == '\n') {
            body_length =  stoll(ostr.str());
            break;
        } else {
            ostr << *header_value_start_iter;
        }
    }

    ByteBuffer_Iterator body_start_iter = http_body_pos[0] + 4; // 跳过分割符 \r\n\r\n
    ByteBuffer_Iterator last_pos = data.last_data();
    if (last_pos - body_start_iter + 1 < body_length) {
        return -1; // 缓存中的报文主体不够
    } else {
        data.get_data(content_, body_start_iter, body_length); // 获取报文主体
    }
    BUFSIZE_T http_packet_size = http_body_pos[0] - http_begin_iter + body_length + 4; // 计算 http 包大小 +4 是为了计入 \r\n\r\n

    ByteBuffer http_headers;
    data.get_data(http_headers, http_begin_iter, http_body_pos[0] - http_begin_iter); // 获取报文首部
    vector<ByteBuffer> headers = http_headers.split(ByteBuffer("\r\n")); // 分割报文首部

    
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
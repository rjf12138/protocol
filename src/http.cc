#include "algorithm/algorithm.h"
#include "protocol.h"

namespace ptl {

#define ITER_INCRE_AND_CHECK(iter, buffer) \
    if ((iter + 1) == buffer.end()) {\
        return HttpParse_ContentNotEnough;\
    } else {\
        ++iter;\
    }

#define ITER_CHECK(iter, buffer, pos) \
    if ((iter + pos) == buffer.end()) {\
        return HttpParse_ContentNotEnough;\
    }

enum HttpParseState {
    HttpParseState_Method,
    HttpParseState_UrlOrRetCode,
    HttpParseState_VersionOrPhrase,
    HttpParseState_HeadOptionKey,
    HttpParseState_HeadOptionValue,
    HttpParseState_ContentBody,
    HttpParseState_TranferEncoding,
    HttpParseState_End
};

enum HttpParseTranferEncodeState {
    HttpParseTranferEncodeState_DataLen,
    HttpParseTranferEncodeState_ContentBody,
    HttpParseTranferEncodeState_ChunkEnd,
    HttpParseTranferEncodeState_End,
};

HttpPtl::HttpPtl(void)
{

}

HttpPtl::~HttpPtl(void)
{
    
}

HttpParse_ErrorCode 
HttpPtl::parse(basic::ByteBuffer &data)
{
    if (data.data_size() <= 14) {
        return HttpParse_ContentNotEnough;
    }
    this->clear();

    is_request_ = true;
    ssize_t content_len = 0;
    std::string key, value;
    auto iter = data.begin();
    HttpParseState state = HttpParseState_Method;
    while (true) {
        switch (state)
        {
            case HttpParseState_Method: 
            {
                switch (*iter) 
                {
                    case 'G':
                    {
                        basic::ByteBuffer patten(HTTP_METHOD_GET);
                        if (data.bytecmp(iter, patten)) {
                            method_ = HTTP_METHOD_GET;
                            iter += patten.data_size();
                            state = HttpParseState_UrlOrRetCode;
                        }
                    } break;
                    case 'P':
                    {
                        basic::ByteBuffer patten(HTTP_METHOD_PUT);
                        if (data.bytecmp(iter, patten)) {
                            method_ = HTTP_METHOD_PUT;
                            iter += patten.data_size();
                            state = HttpParseState_UrlOrRetCode;
                        }

                        patten.clear();
                        patten.write_string(HTTP_METHOD_POST);
                        if (data.bytecmp(iter, patten)) {
                            method_ = HTTP_METHOD_POST;
                            iter += patten.data_size();
                            state = HttpParseState_UrlOrRetCode;
                        }
                    } break;
                    case 'D':
                    {
                        basic::ByteBuffer patten(HTTP_METHOD_DELETE);
                        if (data.bytecmp(iter, patten)) {
                            method_ = HTTP_METHOD_DELETE;
                            iter += patten.data_size();
                            state = HttpParseState_UrlOrRetCode;
                        }
                    } break;
                    case 'H':
                    {
                        basic::ByteBuffer patten(HTTP_METHOD_HEAD);
                        if (data.bytecmp(iter, patten)) {
                            method_ = HTTP_METHOD_HEAD;
                            iter += patten.data_size();
                            state = HttpParseState_UrlOrRetCode;
                        }

                        patten.clear();
                        patten.write_string(HTTP_METHOD_RESPONE);
                        if (data.bytecmp(iter, patten)) {
                            method_ = HTTP_METHOD_RESPONE;
                            iter += patten.data_size();
                            state = HttpParseState_UrlOrRetCode;

                            is_request_ = false;
                        }
                    } break;
                    case 'O':
                    {
                        basic::ByteBuffer patten(HTTP_METHOD_OPTION);
                        if (data.bytecmp(iter, patten)) {
                            method_ = HTTP_METHOD_OPTION;
                            iter += patten.data_size();
                            state = HttpParseState_UrlOrRetCode;
                        }
                    } break;
                    default:
                    {
                        return HttpParse_CantFindHttp;
                    }
                }

                if (state != HttpParseState_UrlOrRetCode) {
                    return HttpParse_CantFindHttp;
                }
            } break;
            case HttpParseState_UrlOrRetCode:
            {
                if (*iter != ' ') {
                    url_ += *iter;
                }

                ITER_INCRE_AND_CHECK(iter, data);

                if (*iter == ' ') {
                    if (is_request_ == false) {
                        code_ = std::stoi(url_);
                    }
                    
                    state = HttpParseState_VersionOrPhrase;
                    while (true) {
                        if (*iter == ' ') {
                            ITER_INCRE_AND_CHECK(iter, data);
                        } else {
                            break;
                        }
                    }
                }
            } break;
            case HttpParseState_VersionOrPhrase:
            {
                phrase_ += *iter;
                ITER_INCRE_AND_CHECK(iter, data);
                
                if (*iter == '\r') {
                    state = HttpParseState_HeadOptionKey;
                    key = "";
                }
            } break;
            case HttpParseState_HeadOptionKey:
            {
                if (*iter == '\r' || *iter == '\n') {
                    ITER_INCRE_AND_CHECK(iter, data);
                    continue;
                } 
                
                key += *iter;
                ITER_INCRE_AND_CHECK(iter, data);

                if (*iter == ':') {
                    state = HttpParseState_HeadOptionValue;
                    value = "";
                }
            } break;
            case HttpParseState_HeadOptionValue:
            {
                if (*iter == ' ' || *iter == ':') {
                    ITER_INCRE_AND_CHECK(iter, data);
                    continue;
                } 
                
                value += *iter;
                ITER_INCRE_AND_CHECK(iter, data);
                ITER_CHECK(iter, data, 2);

                if (*iter == '\r') {
                    if (*(iter + 2) == '\r') {
                        if (header_.find(HTTP_HEADER_TranferEncoding) != header_.end()) {
                            state = HttpParseState_TranferEncoding;
                        } else {
                            state = HttpParseState_ContentBody;
                        }
                        iter += 4; // 指向消息体开始位置
                    } else {
                        state = HttpParseState_HeadOptionKey;
                    }
                    header_[key] = value;
                    if (key == HTTP_HEADER_ContentLength) {
                        content_len = std::stoi(value);
                    }
                    key = "";
                }
            } break;
            case HttpParseState_ContentBody:
            {
                if (content_len == 0) {
                    state = HttpParseState_End;
                    break;
                }
                
                ssize_t ret = data.get_data(content_, iter, content_len);
                if (ret != content_len) {
                    return HttpParse_ContentNotEnough;
                } else {
                    iter += ret;
                    state = HttpParseState_End;
                }
            } break;
            case HttpParseState_TranferEncoding:
            {

            } break;
            case HttpParseState_End:
            {
                auto start_iter = data.begin();
                ssize_t update_size = iter - start_iter;
                data.update_read_pos(update_size);
                return HttpParse_OK;
            } break;
            default:
                break;
        }
    }
    return HttpParse_NotSupportHttp;
}

int 
HttpPtl::generate(basic::ByteBuffer &data)
{
    data.clear();
    if (is_request_) { // 设置请求行
        data.write_string(method_);
        data.write_int8(' ');
        data.write_string(url_);
        data.write_int8(' ');
        data.write_string(HTTP_VERSION);
    } else {    // 设置回复行
        data.write_string(HTTP_VERSION);
        data.write_int8(' ');
        data.write_string(std::to_string(code_));
        data.write_int8(' ');
        data.write_string(phrase_);
    }
    data.write_string("\r\n");

    // 首部选项
    header_[HTTP_HEADER_ContentLength] = std::to_string(content_.data_size()); // 不管外部是否设置，内部必须设置报文主体长度
    for (auto iter = header_.begin(); iter != header_.end(); ++iter) {
        data.write_string(iter->first);
        data.write_string(": ");
        data.write_string(iter->second);
        data.write_string("\r\n");
    }
    data.write_string("\r\n");
    data += content_;

    return 0;
}

int 
HttpPtl::clear()
{
    content_.clear();
    url_ = "";
    method_ = "";
    phrase_ = "";
    header_.clear();
    code_ = 0;

    return 0;
}

int 
HttpPtl::set_content(const basic::ByteBuffer &data)
{
    content_ = data;
    this->set_header_option(HTTP_HEADER_ContentLength, std::to_string(content_.data_size()));
    return 0;
}

int 
HttpPtl::set_request(const std::string &method, const std::string &url)
{
    method_ = method;
    url_ = url;
    is_request_ = true;

    return 0;
}

int 
HttpPtl::set_response(int code, const std::string &phrase)
{
    code_ = code;
    phrase_ = phrase;
    is_request_ = false;

    return 0;
}

int 
HttpPtl::set_phrase(const std::string &phrase)
{
    phrase_ = phrase;
    return 0;
}

int 
HttpPtl::set_header_option(const std::string &key, const std::string &value)
{
    header_[key] = value;

    return 0;
}

int 
HttpPtl::get_status_code(void)
{
    return code_;
}

std::string 
HttpPtl::get_url(void)
{
    return url_;
}

std::string 
HttpPtl::get_method(void)
{
    return method_;
}

std::string 
HttpPtl::get_phrase(void)
{
    return phrase_;
}

std::string 
HttpPtl::get_header_option(const std::string &key)
{
    return header_[key];
}

basic::ByteBuffer&
HttpPtl::get_content(void)
{
    return content_;
}

HttpParse_ErrorCode
HttpPtl::parse_tranfer_encoding(basic::ByteBuffer &data, basic::ByteBufferIterator iter)
{
    uint32_t data_len = 0;
    auto tmp_iter = iter;
    HttpParseTranferEncodeState state = HttpParseTranferEncodeState_DataLen;
    while (tmp_iter != data.end()) {
        switch (state) {
            case HttpParseTranferEncodeState_DataLen:
            {
                if (*tmp_iter >= '0' && *tmp_iter <= '9') {
                    data_len = data_len * 16 + (*tmp_iter - '0');
                } else if (*tmp_iter >= 'A' && *tmp_iter <= 'F') {
                     data_len = data_len * 16 + (*tmp_iter - 'A' + 10);
                } else if (*tmp_iter >= 'a' && *tmp_iter <= 'f') {
                     data_len = data_len * 16 + (*tmp_iter - 'a' + 10);
                } else {
                    ITER_INCRE_AND_CHECK(tmp_iter, data);
                ITER_CHECK(tmp_iter, data, 2);
                if (*tmp_iter == '\r' && *(tmp_iter + 1) == '\n') {
                    state = HttpParseTranferEncodeState_ContentBody;
                    tmp_iter += 2;
                    continue;
                }
                }

                if (*tmp_iter == '\n' && (*(tmp_iter - 1) != '\\' || 
                    *(tmp_iter - 2) != '\r' || *(tmp_iter - 3) != '\\')) {
                        return HttpParse_ParseDataLenUnknownChar;
                } else if (*tmp_iter == '\n' && *(tmp_iter - 1) == '\\' &&
                    *(tmp_iter - 2) == '\r' && *(tmp_iter - 3) == '\\') {

                }
                return HttpParse_ParseDataLenFailed;
            } break;
        }
    }
}

}
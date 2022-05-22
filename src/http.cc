#include "algorithm/algorithm.h"
#include "protocol.h"

namespace ptl {

// 检查能否读下个数据，可以的话移到下一个数据
#define ITER_INCRE_AND_CHECK(iter, buffer) \
    if ((iter + 1) == buffer.end()) {\
        return HttpParse_ContentNotEnough;\
    } else {\
        ++iter;\
    }

// 检查还能读多少数据，不包括当前迭代器指的位置
#define ITER_CHECK_EXCLUDE_ITER(iter, buffer, pos) \
    if ((iter + pos) == buffer.end()) {\
        return HttpParse_ContentNotEnough;\
    }

// 检查还能读多少数据，包括当前迭代器指的位置
#define ITER_CHECK_INCLUDE_ITER(iter, buffer, pos) \
    {\
        auto tmp_pos = pos - 1;\
        if ((iter + tmp_pos) == buffer.end()) {\
            return HttpParse_ContentNotEnough;\
        }\
    }

HttpPtl::HttpPtl(void)
:is_parse_tranfer_encode_(false)
{
     
}

HttpPtl::~HttpPtl(void)
{
    
}

HttpParse_ErrorCode 
HttpPtl::parse(basic::ByteBuffer &data)
{
    if (is_parse_tranfer_encode_ == true) {
        return parse_tranfer_encoding(data);
    }

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
                ITER_CHECK_INCLUDE_ITER(iter, data, 4);

                if (*iter == '\r') {
                    if (*(iter + 2) == '\r') {
                        if (header_.find(HTTP_HEADER_TransferEncoding) != header_.end() && header_[HTTP_HEADER_TransferEncoding] == "chunked") {
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
                is_parse_tranfer_encode_ = true;
                auto start_iter = data.begin();
                ssize_t update_size = iter - start_iter;
                data.update_read_pos(update_size);

                return parse_tranfer_encoding(data);
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
    is_parse_tranfer_encode_ = false;

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

std::vector<basic::ByteBuffer> &
HttpPtl::get_tranfer_encode_datas(void)
{
    return tranfer_encode_datas_;
}

HttpParse_ErrorCode
HttpPtl::parse_tranfer_encoding(basic::ByteBuffer &data)
{
    tranfer_encode_iter_ = data.begin();
    parse_tranfer_encode_state_ = HttpParseTranferEncodeState_DataLen;
    uint32_t data_len = 0;
    while (true) {
        switch (static_cast<int>(parse_tranfer_encode_state_)) {
            case HttpParseTranferEncodeState_DataLen: {
                ITER_CHECK_INCLUDE_ITER(tranfer_encode_iter_, data, 1); // 检查可以读的字节数
                if (*tranfer_encode_iter_ >= '0' && *tranfer_encode_iter_ <= '9') {
                    data_len = data_len * 16 + (*tranfer_encode_iter_ - '0');
                } else if (*tranfer_encode_iter_ >= 'A' && *tranfer_encode_iter_ <= 'F') {
                    data_len = data_len * 16 + (*tranfer_encode_iter_ - 'A' + 10);
                } else if (*tranfer_encode_iter_ >= 'a' && *tranfer_encode_iter_ <= 'f') {
                    data_len = data_len * 16 + (*tranfer_encode_iter_ - 'a' + 10);
                } else {
                    ITER_CHECK_INCLUDE_ITER(tranfer_encode_iter_, data, data_len + 4);
                    if (*tranfer_encode_iter_ == '\r' && *(tranfer_encode_iter_ + 1) == '\n') {
                        parse_tranfer_encode_state_ = HttpParseTranferEncodeState_ContentBody;
                        tranfer_encode_iter_ += 2;
                        continue;
                    } else {
                        return HttpParse_ParseDataLenUnknownChar;
                    }
                }
            } break;
            case HttpParseTranferEncodeState_ContentBody: {
                if (data_len > 0) {
                    content_.clear();
                    data.get_data(content_, tranfer_encode_iter_, data_len);
                    tranfer_encode_datas_.push_back(content_);
                    tranfer_encode_iter_ += data_len;
                }
                if (*tranfer_encode_iter_ == '\r' && *(tranfer_encode_iter_ + 1) == '\n') {
                    parse_tranfer_encode_state_ = HttpParseTranferEncodeState_ChunkEnd;
                    tranfer_encode_iter_ += 2;
                    continue;
                } else {
                    return HttpParse_ParseDataLenUnknownChar;
                }
            } break;
            case HttpParseTranferEncodeState_ChunkEnd: {
                auto start_iter = data.begin();
                ssize_t update_size = tranfer_encode_iter_ - start_iter;
                data.update_read_pos(update_size);
                if (data_len <= 0) {
                    return HttpParse_OK;
                } else {
                    data_len = 0;
                    parse_tranfer_encode_state_ = HttpParseTranferEncodeState_DataLen;
                    continue;
                }
            } break;
        }
        ITER_INCRE_AND_CHECK(tranfer_encode_iter_, data);
    }
    return HttpParse_ContentNotEnough;
}

}
#include "algorithm/algorithm.h"
#include "http.h"

namespace ptl {

HttpPtl::HttpPtl(void)
{

}

HttpPtl::~HttpPtl(void)
{

}

HttpParse_ErrorCode 
HttpPtl::parser(basic::ByteBuffer &data)
{
    is_request_ = true;
    auto start_pos_iter = data.begin();
    auto iter = data.begin();
    for (; iter != data.end(); ++iter) {
        switch (*iter) {
            case 'G':
            {
                basic::ByteBuffer patten(HTTP_METHOD_GET);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_GET;
                    goto __HTTP_HEAD__;
                }
            } break;
            case 'P':
            {
                basic::ByteBuffer patten(HTTP_METHOD_PUT);
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
                basic::ByteBuffer patten(HTTP_METHOD_DELETE);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_DELETE;
                    goto __HTTP_HEAD__;
                }
            } break;
            case 'H':
            {
                basic::ByteBuffer patten(HTTP_METHOD_HEAD);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_HEAD;
                    goto __HTTP_HEAD__;
                }

                patten.clear();
                patten.write_string(HTTP_METHOD_RESPONE);
                if (data.bytecmp(iter, patten)) {
                    method_ = HTTP_METHOD_RESPONE;
                    is_request_ = false;
                    goto __HTTP_HEAD__;
                }
            } break;
            case 'O':
            {
                basic::ByteBuffer patten(HTTP_METHOD_OPTION);
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

    return HttpParse_CantFindHttp; // 查找 HTTP 头失败

__HTTP_HEAD__:
    data.update_read_pos(iter - start_pos_iter); // 更新basic::ByteBuffer读位置到http协议开始的位置
    basic::ByteBuffer::iterator http_begin_iter = data.begin();

    std::ostringstream ostr;
    ssize_t body_length = 0;

    std::vector<basic::ByteBuffer::iterator> http_length_pos = data.find(basic::ByteBuffer(HTTP_HEADER_ContentLength));
    std::vector<basic::ByteBuffer::iterator> http_body_pos = data.find(basic::ByteBuffer("\r\n\r\n"));

    if (http_body_pos.size() <= 0 ) { // 找不到报文主体开始的位置
        return HttpParse_CantFindBody;
    }

    if (http_length_pos.size() <= 0) {
        body_length = 0;
    } else {
        basic::ByteBuffer::iterator header_value_start_iter = http_length_pos[0] + strlen(HTTP_HEADER_ContentLength); // 获取报文主体的长度
        for (; header_value_start_iter != data.end(); ++header_value_start_iter) {
            if (*header_value_start_iter == ':' || *header_value_start_iter == ' ') {
                continue;
            } else if (*header_value_start_iter == '\r' && *(header_value_start_iter+1) == '\n') {
                body_length =  std::stoll(ostr.str());
                break;
            } else {
                ostr << *header_value_start_iter;
            }
        }
    }

    basic::ByteBuffer::iterator body_start_iter = http_body_pos[0] + 4; // 跳过分割符 \r\n\r\n
    basic::ByteBuffer::iterator last_pos = data.last_data();
    if (last_pos - body_start_iter + 1 < body_length) {
        return HttpParse_ContentNotEnough; // 缓存中的报文主体不够
    } else {
        data.get_data(content_, body_start_iter, body_length); // 获取报文主体
    }

    basic::ByteBuffer http_headers;
    data.get_data(http_headers, http_begin_iter, http_body_pos[0] - http_begin_iter); // 获取报文首部
    std::vector<basic::ByteBuffer> headers = http_headers.split(basic::ByteBuffer("\r\n")); // 分割报文首部

    // 解析报文请求行
    bool flag = false;
    ostr.str("");
    auto http_request_iter = headers[0].begin() + method_.length(); // 跳过http方法，前面解析过了
    while (http_request_iter != headers[0].end()) {       // 获取请求行url或是回复的错误码
        if (*http_request_iter == ' ' && flag == false) {
            ++http_request_iter;
            continue;
        } else if (*http_request_iter != ' ' && flag == false) {
            flag = true;
            continue;
        } else if (*http_request_iter != ' ' && flag == true) {
            ostr << *http_request_iter;
            ++http_request_iter;
        } else if ((*http_request_iter == ' ' && flag == true)) {
            if (method_ == HTTP_METHOD_RESPONE) {
                code_ = std::stoll(ostr.str());
            } else {
                url_ = ostr.str();
            }
            break;
        }
    }

    flag = false;
    ostr.str(""); // 获取请求的协议类型，或是回复的消息短语
    while (http_request_iter != headers[0].end()) {
        if (*http_request_iter == ' ' && flag == false) {
            ++http_request_iter;
            continue;
        } else if (*http_request_iter != ' ' && flag == false) {
            flag = true;
        } else if (flag == true) {
            ostr << *http_request_iter;
            ++http_request_iter;
        }
    }

    if (method_ == HTTP_METHOD_RESPONE) {
        phrase_ = ostr.str();
    } else {
        if (ostr.str() != HTTP_VERSION) { // 检查请求协议的版本是不是HTTP/1.1
            return HttpParse_HttpVersionNotMatch;
        }
    }

    // 处理 HTTP 首部字段
    std::string key;
    for (std::size_t i = 1; i < headers.size(); ++i) {
        ostr.str("");
        ssize_t j = 0;
        for (; j < headers[i].data_size(); ++j) {
            if (headers[i][j] == ':') {
                key = ostr.str();
                ++j;
                break;
            }
            ostr << headers[i][j];
        }
        
        if ((j == headers[i].data_size() && headers[i][j-1] != ':') || 
             key == "") {
            return HttpParse_ParseHeaderFailed;
        }

        flag = false;
        ostr.str("");
        for (; j < headers[i].data_size(); ) {
            if (headers[i][j] == ' ' && flag == false) {
                ++j;
                continue;
            } else if (headers[i][j] != ' ' && flag == false) {
                flag = true;
            } else {
                ostr << headers[i][j];
                ++j;
            }
        }
        header_[key] = ostr.str();
    }
    
    // 解析完后，从缓存中移除旧的 http 消息
    ssize_t http_packet_size = http_body_pos[0] - http_begin_iter + body_length + 4; // 计算 http 包大小 +4 是为了计入 \r\n\r\n
    data.update_read_pos(http_packet_size);

    return HttpParse_OK;
}

enum HttpParseState {
    HttpParseState_Method,
    HttpParseState_UrlOrRetCode,
    HttpParseState_VersionOrPhrase,
    HttpParseState_HeadOptionKey,
    HttpParseState_HeadOptionValue,
    HttpParseState_ContentBody,
    HttpParseState_End
};

HttpParse_ErrorCode 
HttpPtl::parse(basic::ByteBuffer &data)
{
    if (data.data_size() <= 14) {
        return HttpParse_ContentNotEnough;
    }

    is_request_ = true;
    ssize_t content_len = -1;
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

                ++iter;
                if (iter == data.end()) {
                    return HttpParse_ContentNotEnough;
                }

                if (*iter == ' ') {
                    if (is_request_ == false) {
                        code_ = std::stoi(url_);
                    }
                    
                    state = HttpParseState_VersionOrPhrase;
                    while (true) {
                        if (*iter == ' ') {
                            ++iter;
                            if (iter == data.end()) {
                                return HttpParse_ContentNotEnough;
                            }
                        } else {
                            break;
                        }
                    }
                }
            } break;
            case HttpParseState_VersionOrPhrase:
            {
                phrase_ += *iter++;
                if (iter == data.end()) {
                    return HttpParse_ContentNotEnough;
                }

                if (*iter == '\r') {
                    state = HttpParseState_HeadOptionKey;
                    key = "";
                }
            } break;
            case HttpParseState_HeadOptionKey:
            {
                if (*iter == '\r' || *iter == '\n') {
                    ++iter;
                    continue;
                } 
                
                key += *iter++;
                if (iter == data.end()) {
                    return HttpParse_ContentNotEnough;
                }

                if (*iter == ':') {
                    state = HttpParseState_HeadOptionValue;
                    value = "";
                }
            } break;
            case HttpParseState_HeadOptionValue:
            {
                if (*iter == ' ' || *iter == ':') {
                    ++iter;
                    continue;
                } 
                
                value += *iter++;
                if (iter == data.end() || iter + 2 == data.end()) {
                    return HttpParse_ContentNotEnough;
                }

                if (*iter == '\r') {
                    if (*(iter + 2) == '\r') {
                        state = HttpParseState_ContentBody;
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
                if (content_len == -1) {
                    return HttpParse_CantFindBody;
                }
                
                ssize_t ret = data.get_data(content_, iter, content_len);
                if (ret != content_len) {
                    return HttpParse_ContentNotEnough;
                } else {
                    iter += ret;
                    state = HttpParseState_End;
                }
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
HttpPtl::get_header_option(const std::string &key)
{
    return header_[key];
}

basic::ByteBuffer&
HttpPtl::get_content(void)
{
    return content_;
}

}
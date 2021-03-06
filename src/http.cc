#include "http/http.h"

namespace my_protocol {

HttpPtl::HttpPtl(void)
{

}

HttpPtl::~HttpPtl(void)
{

}

HttpParse_ErrorCode 
HttpPtl::parser(ByteBuffer &data)
{
    is_request = true;
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
                    is_request = false;
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

    return HttpParse_CantFindHttp; // ?????? HTTP ?????????

__HTTP_HEAD__:
    data.update_read_pos(iter - start_pos_iter); // ??????ByteBuffer????????????http?????????????????????
    ByteBuffer_Iterator http_begin_iter = data.begin();

    ostringstream ostr;
    BUFSIZE_T body_length = 0;

    vector<ByteBuffer_Iterator> http_length_pos = data.find(ByteBuffer(HTTP_HEADER_ContentLength));
    vector<ByteBuffer_Iterator> http_body_pos = data.find(ByteBuffer("\r\n\r\n"));

    if (http_body_pos.size() <= 0 ) { // ????????????????????????????????????
        return HttpParse_CantFindBody;
    }

    if (http_length_pos.size() <= 0) {
        body_length = 0;
    } else {
        ByteBuffer_Iterator header_value_start_iter = http_length_pos[0] + strlen(HTTP_HEADER_ContentLength); // ???????????????????????????
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
    }

    ByteBuffer_Iterator body_start_iter = http_body_pos[0] + 4; // ??????????????? \r\n\r\n
    ByteBuffer_Iterator last_pos = data.last_data();
    if (last_pos - body_start_iter + 1 < body_length) {
        return HttpParse_ContentNotEnough; // ??????????????????????????????
    } else {
        data.get_data(content_, body_start_iter, body_length); // ??????????????????
    }

    ByteBuffer http_headers;
    data.get_data(http_headers, http_begin_iter, http_body_pos[0] - http_begin_iter); // ??????????????????
    vector<ByteBuffer> headers = http_headers.split(ByteBuffer("\r\n")); // ??????????????????

    // ?????????????????????
    bool flag = false;
    ostr.str("");
    auto http_request_iter = headers[0].begin() + method_.length(); // ??????http???????????????????????????
    while (http_request_iter != headers[0].end()) {       // ???????????????url????????????????????????
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
                code_ = stoll(ostr.str());
            } else {
                url_ = ostr.str();
            }
            break;
        }
    }

    flag = false;
    ostr.str(""); // ?????????????????????????????????????????????????????????
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
        if (ostr.str() != HTTP_VERSION) { // ????????????????????????????????????HTTP/1.1
            return HttpParse_HttpVersionNotMatch;
        }
    }

    // ?????? HTTP ????????????
    std::string key;
    for (std::size_t i = 1; i < headers.size(); ++i) {
        ostr.str("");
        BUFSIZE_T j = 0;
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
    
    // ??????????????????????????????????????? http ??????
    BUFSIZE_T http_packet_size = http_body_pos[0] - http_begin_iter + body_length + 4; // ?????? http ????????? +4 ??????????????? \r\n\r\n
    data.update_read_pos(http_packet_size);

    return HttpParse_OK;
}

int 
HttpPtl::generate(ByteBuffer &data)
{
    data.clear();
    if (is_request) { // ???????????????
        data.write_string(method_);
        data.write_int8(' ');
        data.write_string(url_);
        data.write_int8(' ');
        data.write_string(HTTP_VERSION);
    } else {    // ???????????????
        data.write_string(HTTP_VERSION);
        data.write_int8(' ');
        data.write_string(std::to_string(code_));
        data.write_int8(' ');
        data.write_string(phrase_);
    }
    data.write_string("\r\n");

    // ????????????
    header_[HTTP_HEADER_ContentLength] = std::to_string(content_.data_size()); // ???????????????????????????????????????????????????????????????
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

    return 0;
}

int 
HttpPtl::set_content(ByteBuffer &data)
{
    content_ = data;

    return 0;
}

int 
HttpPtl::set_request(const string &method, const string &url)
{
    method_ = method;
    url_ = url;
    is_request = true;

    return 0;
}

int 
HttpPtl::set_response(int code, const string &phrase)
{
    code_ = code;
    phrase_ = phrase;
    is_request = false;

    return 0;
}

int 
HttpPtl::set_phrase(string phrase)
{
    phrase_ = phrase;
    return 0;
}

int 
HttpPtl::set_header_option(const string &key, const string &value)
{
    header_[key] = value;

    return 0;
}

int 
HttpPtl::get_status_code(void)
{
    return code_;
}

string 
HttpPtl::get_method(void)
{
    return method_;
}

string 
HttpPtl::get_header_option(const string &key)
{
    return header_[key];
}

BUFSIZE_T 
HttpPtl::get_content(ByteBuffer &data)
{
    data = content_;
    return data.data_size();
}

}
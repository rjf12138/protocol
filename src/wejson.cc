#include "wejson.h"

namespace my_util {
WeJson::WeJson(void) = default;
WeJson::WeJson(string str) 
{
    this->parser_from_json(str);
}

WeJson::~WeJson(void)
{

}

string 
WeJson::debug_info(void)
{
    return "";
}

int
WeJson::open_json(string json_file_path)
{
    FileOperate json_file;
    if (json_file.open(json_file_path) == -1) {
        string err_info = get_msg("File: %s is not exists.", json_file_path.c_str());
        throw runtime_error(err_info);
    }

    ByteBuffer raw_json_buffer;
    json_file.read(raw_json_buffer, json_file.file_size());
    return this->parser_from_json(raw_json_buffer);
}

int 
WeJson::write_json(string json_file_path)
{
    FileOperate json_file;
    if (json_file.open(json_file_path) == -1) {
        string err_info = get_msg("File: %s is not exists.", json_file_path.c_str());
        throw runtime_error(err_info);
    }

    ByteBuffer buff;
    buff.write_bytes(this->format_json().c_str(), this->format_json().length());
    json_file.clear_file();
    json_file.write(buff, buff.data_size());

    return 0;
}

int 
WeJson::parser_from_json(string str)
{
    ByteBuffer raw_json_buffer;
    raw_json_buffer.clear();
    raw_json_buffer.write_bytes(str.c_str(), str.length());
    return this->parser_from_json(raw_json_buffer);
}

int 
WeJson::parser_from_json(ByteBuffer &buff)
{
    ByteBuffer simple_json_text;
    ByteBuffer_Iterator start_pos;
    for (start_pos = buff.begin(); start_pos != buff.end(); ++start_pos) {
        VALUE_TYPE ret = JsonType::check_value_type(start_pos);
        if (ret == JSON_OBJECT_TYPE || ret == JSON_ARRAY_TYPE) {
            break;
        }
    }
    if (start_pos == buff.end()) {
        return -1;
    }

    int i = 0;
    bool quotation_marks = false;
    for (auto iter = start_pos; iter != buff.end(); ++iter) {
        if (quotation_marks == false && *iter == '"') {
            quotation_marks = true;
            simple_json_text.write_int8(*iter); // 写入字符串的第一个 "\""
            continue;
        }
        if (quotation_marks == true) {
            if (*iter == '\\'){ 
                simple_json_text.write_int8(*iter);
                ++iter; // '\' 为转义字符下一个字符不做解析
                simple_json_text.write_int8(*iter);
            } else if (*iter == '"') {
                quotation_marks = false;
            }
        }
        for (i = 0; i < 4; ++i) {
            if (sperate_chars[i] == *iter) {
                break;
            }
        }
        if (i < 4 && quotation_marks == false) {
            continue;
        }
        simple_json_text.write_int8(*iter);
    }
    auto begin_json = simple_json_text.begin();
    auto end_json = simple_json_text.end();

    this->parse(begin_json, end_json);
    // VALUE_TYPE ret_type = this->check_value_type(begin_json);
    // if (ret_type == JSON_ARRAY_TYPE) {
    //     json_array_value_.parse(begin_json, end_json);
    //     json_value_type_ = JSON_ARRAY_TYPE;
    // } else if (ret_type == JSON_OBJECT_TYPE) {
    //     json_object_value_.parse(begin_json, end_json);
    //     json_value_type_ = JSON_OBJECT_TYPE;
    // } else {
    //     string err_str = get_msg("Unknown json type (object or array)");
    //     throw runtime_error(err_str);
    // }

    return 0;
}

}
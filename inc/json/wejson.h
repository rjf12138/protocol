#ifndef __WEJSON_H__
#define __WEJSON_H__

#include "basic_head.h"
#include "buffer/byte_buffer.h"
// #include "file.h"
#include "json/json_type.h"

namespace my_utils {

class WeJson : public ValueTypeCast {
public:
    WeJson(void);
    WeJson(string str);
    ~WeJson(void);

    // 解析字符串中保存的json文本
    int parser_from_json(string str);
    // 解析ByteBuffer保存的json文本
    int parser_from_json(ByteBuffer &buff);

    // 当出错时输出调试信息
    string debug_info(void);

public:
    
};

}

#endif
#ifndef __JSON_TYPE_H__
#define __JSON_TYPE_H__

#include "basic_head.h"
#include "buffer/byte_buffer.h"
#include "msg_record.h"

namespace my_protocol {
using namespace my_utils;
// json中重要的分割字符
const vector<char> sperate_chars = {' ', '\r', '\n','\t','{', '}','[', ']',',',':','"'};

class ValueTypeCast;

enum VALUE_TYPE {
    JSON_UNKNOWN_TYPE = -1000,
    JSON_NUMBER_TYPE = 10001,
    JSON_BOOL_TYPE = 10002,
    JSON_NULL_TYPE = 10003,
    JSON_STRING_TYPE = 10004,
    JSON_ARRAY_TYPE = 10005,
    JSON_OBJECT_TYPE = 10006
};

enum NUMBER_TYPE_ {
    NONE_TYPE = -1001,
    INT32_TYPE = 10030,
    DOUBLE_TYPE = 10031,
};

class JsonIndex {
public:
    JsonIndex(void);
    JsonIndex(const uint32_t &index);
    JsonIndex(const int32_t &index);
    JsonIndex(const string &key);
    JsonIndex(const char *key);
    ~JsonIndex(void);

    operator uint32_t();
    operator string();

    JsonIndex& operator=(const uint32_t &index);
    JsonIndex& operator=(const string &key);

    VALUE_TYPE get_type() const {return index_type_;}
private:
    VALUE_TYPE index_type_; // 只能是字符串型和数字型
    uint32_t index_;
    string key_;
};

class JsonIter : public MsgRecord {
    typedef map<string, ValueTypeCast>::iterator ObjIter;
    typedef map<string, ValueTypeCast>::const_iterator ConstObjIter;
    typedef vector<ValueTypeCast>::iterator ArrIter;
    typedef vector<ValueTypeCast>::const_iterator ConstArrIter;
public:
    JsonIter(void);
    JsonIter(const ObjIter &obj_iter);
    JsonIter(const ArrIter &arr_iter);
    ~JsonIter(void);

    operator ObjIter() const;
    operator ConstObjIter() const;
    operator ArrIter() const;
    operator ConstArrIter() const;

    JsonIter& operator=(const ObjIter &iter);
    JsonIter& operator=(const ArrIter &iter);
    bool operator==(const JsonIter &lhs);
    bool operator!=(const JsonIter &lhs);
    
    // 前置 ++
    JsonIter& operator++(void);
    // 后置 ++
    JsonIter operator++(int);
    // 前置 --
    JsonIter& operator--(void);
    // 后置 --
    JsonIter operator--(int);

    // 访问当前迭代器的值，数组first返回""
    string first();
    ValueTypeCast& second();

    VALUE_TYPE get_iter_type() const {return iter_type_;}

private:
    VALUE_TYPE iter_type_; // 只能是字符串型和数字型
    pair<string, ValueTypeCast*> ret_;
    ObjIter obj_iter_;
    ArrIter arr_iter_;
};

class JsonType : public MsgRecord {
public:
    JsonType(void) {}
    virtual ~JsonType(void) {}

    // 检查当前位置的字符来判断接下来的是什么类型，具体参考doc中的资料
    static VALUE_TYPE check_value_type(ByteBuffer_Iterator &iter);
    
    // 根据输入的缓存，解析成json
    int parse(ByteBuffer &data) {return 0;}

    // 解析遇到的类型，具体取决于check_value_type()返回的类型和继承该类的实现
    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos){return ByteBuffer_Iterator();}
    
    // 将json值反序列化为字符串输出， 没有格式化
    virtual string generate(void) { return "";}
};

// json 数值类型
class JsonNumber : public JsonType {
    friend ostream& operator<<(ostream &os, JsonNumber &rhs);
public:
    explicit JsonNumber(void);
    explicit JsonNumber(const double &val);
    ~JsonNumber(void);

    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos) override;
    virtual string generate(void) override;

    double value(void) const  {return value_;}
    operator double() {return value_;}

    bool operator==(const JsonNumber& rhs) const;
    bool operator!=(const JsonNumber& rhs) const;
    bool operator==(const double& rhs) const;
    bool operator!=(const double& rhs) const;
    JsonNumber& operator=(JsonNumber rhs);

private:
    double        value_;
};

// json 布尔类型
class JsonBool : public JsonType {
    friend ostream& operator<<(ostream &os, JsonBool &rhs);
public:
    explicit JsonBool(void);
    explicit JsonBool(bool val);
    ~JsonBool(void);

    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos) override;
    virtual string generate(void) override;

    bool value(void) const  {return value_;}
    operator bool() {return value_;}

    bool operator==(const JsonBool& rhs) const;
    bool operator!=(const JsonBool& rhs) const;
    bool operator==(const bool& rhs) const;
    bool operator!=(const bool& rhs) const;
    JsonBool& operator=(JsonBool rhs);

private:
    bool value_;
};

// json null 类型
class JsonNull : public JsonType {
    friend ostream& operator<<(ostream &os, JsonNull &rhs);
public:
    explicit JsonNull(void);
    explicit JsonNull(string val);
    ~JsonNull(void);

    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos) override;
    virtual string generate(void) override;

    bool operator==(const JsonNull& rhs) const;
    bool operator!=(const JsonNull& rhs) const;
    JsonNull& operator=(JsonNull rhs);

private:
    string value_;
};

// json 字符串类型
class JsonString : public JsonType {
    friend ostream& operator<<(ostream &os, JsonString &rhs);
public:
    explicit JsonString(void);
    explicit JsonString(string val);
    explicit JsonString(const char *val);
    ~JsonString(void);

    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos) override;
    virtual string generate(void) override;

    string value(void) const  {return value_;}
    operator std::string() {return value_;}

    bool operator==(const JsonString& rhs) const;
    bool operator!=(const JsonString& rhs) const;
    bool operator==(const string& rhs) const;
    bool operator!=(const string& rhs) const;
    JsonString& operator=(JsonString rhs);

private:
    string value_;
};

// json 对象类型
class JsonObject : public JsonType {
    friend ostream& operator<<(ostream &os, JsonObject &rhs);
public:
    explicit JsonObject(void);
    ~JsonObject(void);

    // 序列化和反序列化
    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos) override;
    virtual string generate(void) override;
    
    // 查找元素
    JsonIter find(const string &key);
    // 操作元素
    int erase(JsonIndex &key);
    // 当前类型为对象时添加元素
    int add(JsonIndex key, ValueTypeCast value);
    // 返回元素数量
    int size(void) const {return object_val_.size();}

    // 重载操作符
    bool operator==(const JsonObject& rhs) const;
    bool operator!=(const JsonObject& rhs) const;
    JsonObject& operator=(JsonObject rhs);
    ValueTypeCast& operator[](const string &key);

public:
    map<string, ValueTypeCast> object_val_;
};

// json 数组类型
class JsonArray : public JsonType {
    friend ostream& operator<<(ostream &os, JsonArray &rhs);
public:
    explicit JsonArray(void);
    ~JsonArray(void);

    // 序列化和反序列化
    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos) override;
    virtual string generate(void) override;

    // 数组或是对象删除元素
    int erase(JsonIndex index);
    // 当前添加元素
    int add(ValueTypeCast value);
    // 返回元素数量
    int size(void) const {return array_val_.size();}

    // 重载操作符
    ValueTypeCast& operator[](size_t key);
    const ValueTypeCast& operator[](const size_t key) const;
    bool operator==(const JsonArray& rhs) const;
    bool operator!=(const JsonArray& rhs) const; 
    JsonArray& operator=(JsonArray rhs);

public:
    vector<ValueTypeCast> array_val_;
};

// json中转类型：可以安装当前存储的类型输出或是接收不同的类型
class ValueTypeCast : public JsonType {
    friend ostream& operator<<(ostream &os, ValueTypeCast &rhs);
    friend JsonObject;
    friend JsonArray;
public:
    ValueTypeCast(void);
    ValueTypeCast(const JsonBool &value);
    ValueTypeCast(const JsonNumber &value);
    ValueTypeCast(const JsonString &value);
    ValueTypeCast(const JsonObject &value);
    ValueTypeCast(const JsonArray &value);
    ValueTypeCast(const JsonNull &value);
    
    ValueTypeCast(const bool &value);
    ValueTypeCast(const int &value);
    ValueTypeCast(const double &value);
    ValueTypeCast(const string &value);
    ValueTypeCast(const char *value);

    ValueTypeCast(const ValueTypeCast& value); // 只能使用引用不然会报错
    ~ValueTypeCast(void);

    operator JsonNumber();
    operator JsonString();
    operator JsonBool();
    operator JsonObject();
    operator JsonArray();
    operator JsonNull();

    ValueTypeCast& operator=(JsonBool val);
    ValueTypeCast& operator=(JsonNumber val);
    ValueTypeCast& operator=(JsonString val);
    ValueTypeCast& operator=(JsonObject val);
    ValueTypeCast& operator=(JsonArray val);
    ValueTypeCast& operator=(JsonNull val);
    ValueTypeCast& operator=(ValueTypeCast val);

    bool operator==(const ValueTypeCast &rhs) const;
    bool operator!=(const ValueTypeCast &rhs) const;
   
    // 访问json值
    ValueTypeCast& operator[](JsonIndex key);

    // 解析json类型
    virtual ByteBuffer_Iterator parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos);
    // 非格式化输出 json
    virtual string generate(void);
    // 格式化输出 json
    virtual string format_json(void);

    // 查找元素（只能同级查找）
    JsonIter find(const string &key);
    // 移除元素元素
    int erase(JsonIndex key);
    // 对象添加元素
    int add(JsonIndex key, ValueTypeCast value);
    // 数组添加元素
    int add(ValueTypeCast value);
    // 返回元素数量
    int size(void);
    
    // 清空所有元素
    int clear(void);

    VALUE_TYPE get_type(void) const {return json_value_type_;}
    
    // 只有Json的数组或对象类型支持
    // 返回数组或是对象的起始和结束迭代器
    JsonIter begin(void);
    JsonIter end(void);

private:
    VALUE_TYPE json_value_type_;
    JsonArray json_array_value_;
    JsonObject json_object_value_;
    JsonString json_string_value_;
    JsonNumber json_number_value_;
    JsonBool   json_bool_value_;
    JsonNull json_null_value_;
};

class WeJson : public ValueTypeCast 
{
public:
    WeJson(void);
    WeJson(const string &json);
    WeJson(ByteBuffer &data);
    virtual ~WeJson(void);

    // 解析输入json文本
    virtual int parse(ByteBuffer &data);
    virtual int parse(const string &data);

    // 非格式化输出 json
    virtual string generate(void);
    // 格式化输出 json
    virtual string format_json(void);
};

}

#endif
#include "json/wejson.h"

namespace my_utils {

JsonIndex::JsonIndex(void)
 : index_type_(JSON_NUMBER_TYPE),
   index_(0),
   key_("")
   {}

JsonIndex::JsonIndex(const uint32_t &index)
 : index_type_(JSON_NUMBER_TYPE),
   index_(index),
   key_("")
   {}

JsonIndex::JsonIndex(const int32_t &index)
 : index_type_(JSON_NUMBER_TYPE),
   index_(index),
   key_("")
   {}

JsonIndex::JsonIndex(const string &key)
 : index_type_(JSON_STRING_TYPE),
   index_(0),
   key_(key)
   {}
JsonIndex::JsonIndex(const char *key)
 : index_type_(JSON_STRING_TYPE),
   index_(0),
   key_(key)
   {}
JsonIndex::~JsonIndex(void)
{
}

JsonIndex::operator uint32_t()
{
    if (index_type_ == JSON_NUMBER_TYPE) {
        return index_;
    }

    return 0;
}
JsonIndex::operator string()
{
    if (index_type_ == JSON_STRING_TYPE) {
        return key_;
    }

    return "";
}

JsonIndex& 
JsonIndex::operator=(const uint32_t &index)
{
    index_type_ = JSON_NUMBER_TYPE;
    index_ = index;

    return *this;
}

JsonIndex& 
JsonIndex::operator=(const string &key)
{
    index_type_ = JSON_STRING_TYPE;
    key_ = key;

    return *this;
}

//////////////////////////////////JsonIter////////////////////////////
JsonIter::JsonIter(void)
    : iter_type_(JSON_NULL_TYPE)
{}
JsonIter::JsonIter(const ObjIter &obj_iter)
    : iter_type_(JSON_OBJECT_TYPE), obj_iter_(obj_iter)
{}
JsonIter::JsonIter(const ArrIter &arr_iter)
    : iter_type_(JSON_ARRAY_TYPE), arr_iter_(arr_iter)
{}
JsonIter::~JsonIter(void) 
{}

JsonIter::operator ObjIter() const
{
    if (iter_type_ != JSON_OBJECT_TYPE) {
        return obj_iter_; // 未定义行为
    }

    return obj_iter_;
}

JsonIter::operator ConstObjIter() const
{
    if (iter_type_ != JSON_OBJECT_TYPE) {
        return obj_iter_; // 未定义行为
    }

    return obj_iter_;
}

JsonIter::operator ArrIter() const
{
    if (iter_type_ != JSON_ARRAY_TYPE) {
        return arr_iter_; // 未定义行为
    }

    return arr_iter_;
}

JsonIter::operator ConstArrIter() const
{
    if (iter_type_ != JSON_ARRAY_TYPE) {
        return arr_iter_; // 未定义行为
    }

    return arr_iter_;
}

JsonIter& 
JsonIter::operator=(const ObjIter &iter)
{
    iter_type_ = JSON_OBJECT_TYPE;
    obj_iter_ = iter;

    return *this;
}
JsonIter& 
JsonIter::operator=(const ArrIter &iter)
{
    iter_type_ = JSON_ARRAY_TYPE;
    arr_iter_ = iter;

    return *this;
}

bool 
JsonIter::operator==(const JsonIter &lhs)
{
    if (iter_type_ != JSON_ARRAY_TYPE && iter_type_ != JSON_OBJECT_TYPE) {
        return false;
    }

    if (lhs.iter_type_ != JSON_OBJECT_TYPE && lhs.iter_type_ != JSON_ARRAY_TYPE) {
        return false;
    }

    if (iter_type_ != lhs.iter_type_) {
        return false;
    }

    if (iter_type_ == JSON_ARRAY_TYPE && arr_iter_ == lhs.arr_iter_) {
        return true;
    }

    if (iter_type_ == JSON_OBJECT_TYPE && obj_iter_ == lhs.obj_iter_) {
        return true;
    }

    return false;
}

bool 
JsonIter::operator!=(const JsonIter &lhs)
{
    return !(*this == lhs);
}

// 前置 ++
JsonIter& 
JsonIter::operator++(void)
{
    if (iter_type_ == JSON_OBJECT_TYPE) {
        ++obj_iter_;
    } else if (iter_type_ == JSON_ARRAY_TYPE) {
        ++arr_iter_;
    } else {
        string err_str = GET_MSG("unknown type: call JsonIter::operator++(void) failed!");
        throw runtime_error(err_str);
    }

    return *this;
}

JsonIter 
JsonIter::operator++(int)
{
    JsonIter tmp = *this;
    if (iter_type_ == JSON_OBJECT_TYPE) {
        ++obj_iter_;
    } else if (iter_type_ == JSON_ARRAY_TYPE) {
        ++arr_iter_;
    } else {
        string err_str = GET_MSG("unknown type: call JsonIter::operator++(int) failed!");
        throw runtime_error(err_str);
    }

    return tmp;
}
JsonIter& 
JsonIter::operator--(void)
{
    if (iter_type_ == JSON_OBJECT_TYPE) {
        --obj_iter_;
    } else if (iter_type_ == JSON_ARRAY_TYPE) {
        --arr_iter_;
    } else {
        string err_str = GET_MSG("unknown type: call JsonIter::operator--(void) failed!");
        throw runtime_error(err_str);
    }

    return *this;
}
JsonIter 
JsonIter::operator--(int)
{
    JsonIter tmp = *this;
    if (iter_type_ == JSON_OBJECT_TYPE) {
        --obj_iter_;
    } else if (iter_type_ == JSON_ARRAY_TYPE) {
        --arr_iter_;
    } else {
        string err_str = GET_MSG("unknown type: call JsonIter::operator--(int) failed!");
        throw runtime_error(err_str);
    }

    return tmp;
}

string
JsonIter::first()
{
    if (iter_type_ == JSON_OBJECT_TYPE) {
        return obj_iter_->first;
    } else if (iter_type_ == JSON_ARRAY_TYPE) {
        return "";
    } else {
        string err_str = GET_MSG("unknown type: call JsonIter::operator*() failed!");
        throw runtime_error(err_str);
    }
}
ValueTypeCast& 
JsonIter::second()
{
    if (iter_type_ == JSON_OBJECT_TYPE) {
        return obj_iter_->second;
    } else if (iter_type_ == JSON_ARRAY_TYPE) {
        return *arr_iter_;
    } else {
        string err_str = GET_MSG("unknown type: call JsonIter::operator*() failed!");
        throw runtime_error(err_str);
    }
}

/////////////////////////////////////////////////////////
VALUE_TYPE
JsonType::check_value_type(ByteBuffer_Iterator &iter) 
{
    if (iter == iter.end()){
        return UNKNOWN_TYPE;
    }
    if (isdigit(*iter) || *iter == '-' || *iter == '+') {
        return JSON_NUMBER_TYPE;
    }

    switch (*iter)
    {
        case '{':
        {
            return JSON_OBJECT_TYPE;
        } break;
        case '[':
        {
            return JSON_ARRAY_TYPE;
        } break;
        case '"':
        {
            return JSON_STRING_TYPE;
        } break;
        case 't':
        {
            return JSON_BOOL_TYPE;
        }break;
        case 'f':
        {
            return JSON_BOOL_TYPE;
        }break;
        case 'n':
        {
            return JSON_NULL_TYPE;
        } break;
        default:
            break;
    }

    return UNKNOWN_TYPE;
}

///////////////////////////////////////////////////////////

JsonNumber::JsonNumber(void)
: value_(0)
  
{

}

JsonNumber::JsonNumber(double val)
: value_(val)
{
}

JsonNumber::JsonNumber(const JsonNumber& val)
: value_(val.value_)
{

}

JsonNumber::~JsonNumber(void) {}

ByteBuffer_Iterator 
JsonNumber::parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos)
{
    ByteBuffer_Iterator iter = value_start_pos;
    string str;
    bool is_negative = false;

    if (*iter == '-') {
        is_negative = true;
        ++iter;
    } else if (*iter == '+') {
        is_negative = false;
        ++iter;
    }

    if (iter != json_end_pos && *iter == '0') {
        if (isdigit(*(iter+1))) { // 除了小数和数值0之外，零不能作为第一个数
            string err_str = GET_MSG("Zero can't be first number of integer!");
            throw runtime_error(err_str);
        }
    }

    for (; iter != json_end_pos && *iter != ','; ++iter) {
        if (!isdigit(*iter) && *iter != '.') {
            break;
        }

        str += *iter;
    }

    value_ = stod(str) * (is_negative?-1:1);

    return iter;
}

string 
JsonNumber::generate(void) 
{
    char buf[256] = {0};
    snprintf(buf, 256, "%lf", value_);

    int j = 255;
    for (; j >=0; --j) {
        if (buf[j] == '.') {
            break;
        }
    }

    for (int i = 255; j >= 0 && i >= j; --i) {
        if (buf[i] <= '9' && buf[i] > '0') {
            break;
        }
        buf[i] = '\0';
    }

    return buf;
}

ostream& 
operator<<(ostream &os, JsonNumber &rhs)
{
    os << rhs.generate();

    return os;
}

bool
JsonNumber::operator==(const JsonNumber& rhs) const
{
   if (value_ == rhs.value_) {
       return true;
   }

    return false;
}
bool 
JsonNumber::operator!=(const JsonNumber& rhs) const
{
    return !(*this == rhs);
}

JsonNumber& 
JsonNumber::operator=(JsonNumber rhs)
{
    value_ = rhs.value_;
    return *this;
}

/////////////////////////////////////////////////////////////////

JsonBool::JsonBool(void)
{

}

JsonBool::JsonBool(bool val)
{
    value_ = val;
}
JsonBool::~JsonBool(void)
{

}

ByteBuffer_Iterator 
JsonBool::parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos)
{
    ByteBuffer_Iterator iter = value_start_pos;
    int read_size = 0;
    if (*iter == 't') {
        read_size = 4;
    } else if (*iter == 'f') {
        read_size = 5;
    }

    string str;
    for (int i = 0; iter != json_end_pos && i < read_size; ++iter, ++i) {
        str += *iter;
    }
    
    if (str == "true") {
        value_ = true;
    } else if (str == "false") {
        value_ = false;
    } else {
        throw runtime_error(GET_MSG("Expected true or false, but result is %s", str.c_str()));
    }

    return iter;
}

string 
JsonBool::generate(void)
{
    return value_ == true? "true":"false";
}

ostream& operator<<(ostream &os, JsonBool &rhs)
{
    os << rhs.generate();

    return os;
}

bool 
JsonBool::operator==(const JsonBool& rhs) const
{
    return value_ == rhs.value_?true:false;
}

bool 
JsonBool::operator!=(const JsonBool& rhs) const
{
    return !(*this == rhs);
}


JsonBool& 
JsonBool::operator=(JsonBool rhs)
{
    value_ = rhs.value_;

    return *this;
}


//////////////////////////////////////////////////////////////
JsonNull::JsonNull(void)
{

}
JsonNull::JsonNull(string val)
{
    value_ = val;
}
JsonNull::~JsonNull(void)
{}

ByteBuffer_Iterator 
JsonNull::parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos)
{
    ByteBuffer_Iterator iter = value_start_pos;
    int read_size = 4; // null is four bytes.

    string str;
    for (int i = 0; iter != json_end_pos && i < read_size; ++iter, ++i) {
        str += *iter;
    }

    if (str == "null") {
        value_ = str;
    } else {
        throw runtime_error(GET_MSG("Expected null, but result is %s", str.c_str()));
    }


    return iter;
}

string 
JsonNull::generate(void)
{
    return "null";
}

ostream& operator<<(ostream &os, JsonNull &rhs)
{
    os << rhs.generate();

    return os;
}

bool 
JsonNull::operator==(const JsonNull& rhs) const
{
    return (value_ == "null" ? true : false);
}

bool 
JsonNull::operator!=(const JsonNull& rhs) const
{
    return !(*this == rhs);
}


JsonNull& 
JsonNull::operator=(JsonNull rhs)
{
    value_ = rhs.value_;

    return *this;
}


///////////////////////////////////////////////////////////

JsonString::JsonString(void) {}
JsonString::JsonString(string val): value_(val) {}
JsonString::JsonString(const char *val): value_(val) {};
JsonString::~JsonString(void) {}

ByteBuffer_Iterator 
JsonString::parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos)
{
    ByteBuffer_Iterator iter = value_start_pos;
    ++iter; // 此时 *iter 值为 ",指向字符串的第一个字符
    string str;
    for (; iter != json_end_pos; ++iter) {
        if (*iter == '\\') { 
            str += *iter;
            ++iter; // '\' 为转义字符下一个字符不做解析
            str += *iter;
        } else if (*iter == '"') {
            break;
        }   
        str += *iter;
    }

    if (iter == json_end_pos)
    {
        throw runtime_error(GET_MSG("String need to surround by \"\""));
    }
    value_ = str;
    ++iter; //  iter 指向下一个字符
    return iter;
}

string 
JsonString::generate(void)
{
    return value_;
}

ostream& operator<<(ostream &os, JsonString &rhs)
{
    os << rhs.generate();

    return os;
}

bool JsonString::operator==(const JsonString& rhs) const
{
    if (value_ != rhs.value_) {
        return false;
    }

    return true;
}

bool JsonString::operator!=(const JsonString& rhs) const
{
    return !(*this == rhs);
}

JsonString& 
JsonString::operator=(JsonString rhs)
{
    value_ = rhs.value_;

    return *this;
}

///////////////////////////////////////////////////////////

JsonObject::JsonObject(void) {}
JsonObject::~JsonObject(void) {}

ByteBuffer_Iterator 
JsonObject::parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos)
{
    ByteBuffer_Iterator iter = value_start_pos;
    ++iter; // 由指向'{'改到指向第一个键值对

    string value_name;
    bool flag = false;
    for (; iter != json_end_pos && *iter != '}'; ++iter) {
        VALUE_TYPE ret_value_type = this->check_value_type(iter);
        if (ret_value_type == UNKNOWN_TYPE) {
            if (*iter != ',' && *iter != ':' && *iter != ']') {
                throw runtime_error( GET_MSG("Unknown character in object: %c", *iter));
            }
            continue;
        }

        JsonString json_str;
        if (flag == false && ret_value_type == JSON_STRING_TYPE) {
            iter = json_str.parse(iter, json_end_pos);
            value_name = json_str.generate();
            flag = true;
            continue;
        }

        ValueTypeCast vtc;
        switch (ret_value_type)
        {
            case JSON_OBJECT_TYPE:
            {
                JsonObject val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_ARRAY_TYPE:
            {
                JsonArray val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_STRING_TYPE:
            {
                JsonString val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_BOOL_TYPE:
            {
                JsonBool val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_NULL_TYPE:
            {
                JsonNull val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_NUMBER_TYPE:
            {
                JsonNumber val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
        default:
            break;
        }

        if (object_val_.find(value_name) == object_val_.end()) {
            object_val_[value_name] = vtc;
        } else {
            throw runtime_error(GET_MSG("There's already \"%s\" exists."));
        }
        flag = false;
        if (*iter == '}') { // 有些解析完就直接指向'}'， 如果不退出在回到循环会因值自增错过
            break;
        }
    }

    if (iter != json_end_pos && *iter == '}') {
        ++iter;
    }
    
    return iter;
}

string 
JsonObject::generate(void)
{
    ostringstream output_obj;
    output_obj << "{";
    for (auto iter = object_val_.begin(); iter != object_val_.end(); ++iter) {
        if (iter != object_val_.begin()) {
            output_obj << ",";
        }

        output_obj << "\"" << iter->first << "\"";
        output_obj << ":";
        if (iter->second.json_value_type_ == JSON_ARRAY_TYPE ||
                iter->second.json_value_type_ == JSON_OBJECT_TYPE) {
            output_obj << (iter->second).generate();
        } else {
            if (iter->second.json_value_type_ == JSON_STRING_TYPE) {
                output_obj << "\"" << (iter->second).generate() << "\"";
            } else {
                output_obj << (iter->second).generate();
            }
        }
    }
    output_obj << "}";

    return output_obj.str();
}

JsonIter
JsonObject::find(const string &key)
{
    auto iter = object_val_.find(key);
    return iter;
}

int
JsonObject::erase(JsonIndex &index)
{
    return object_val_.erase(index);
}

int JsonObject::add(JsonIndex key, ValueTypeCast value)
{
    object_val_[key] = value;

    return 1;
}

ostream& operator<<(ostream &os, JsonObject &rhs)
{
    os << rhs.generate();

    return os;
}

bool 
JsonObject::operator==(const JsonObject& rhs) const
{
    if (this->object_val_.size() != rhs.object_val_.size()) {
        return false;
    }

    auto iter_this = this->object_val_.begin();
    auto iter_rhs = rhs.object_val_.begin();
    while (true) {
        if (iter_rhs == rhs.object_val_.end() && iter_this == this->object_val_.end()) {
            return true;
        }

        if (iter_this->first != iter_rhs->first) {
            return false;
        }

        if (iter_this->second != iter_rhs->second) {
            return false;
        }
    }

    return false;
}

bool 
JsonObject::operator!=(const JsonObject& rhs) const
{
    return !(*this == rhs);
}

ValueTypeCast& 
JsonObject::operator[](const string &key)
{
    auto iter = object_val_.find(key);
    if (iter == object_val_.end()) {
        string err_str = GET_MSG("Can't find value in JsonObject with key(%s)", key.c_str());
        throw runtime_error(err_str);
    }
    return object_val_[key];
}

JsonObject& 
JsonObject::operator=(JsonObject rhs)
{
    object_val_ = rhs.object_val_;

    return *this;
}

////////////////////////////////////////////////////////////

JsonArray::JsonArray(void){}
JsonArray::~JsonArray(void){}

ByteBuffer_Iterator 
JsonArray::parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos)
{
    ByteBuffer_Iterator iter = value_start_pos;
    ++iter;

    ValueTypeCast ret_value;
    for (; iter != json_end_pos && *iter != ']'; ++iter) {
        VALUE_TYPE ret_value_type = this->check_value_type(iter);
        if (ret_value_type == UNKNOWN_TYPE) {
            if (*iter != ',') {
                throw runtime_error(GET_MSG("Unknown character in array: %c", *iter));
            }
            continue;
        }
        ValueTypeCast vtc;
        switch (ret_value_type)
        {
            case JSON_OBJECT_TYPE:
            {
                JsonObject val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_ARRAY_TYPE:
            {
                JsonArray val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_STRING_TYPE:
            {
                JsonString val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_BOOL_TYPE:
            {
                JsonBool val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_NULL_TYPE:
            {
                JsonNull val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
            case JSON_NUMBER_TYPE:
            {
                JsonNumber val;
                iter = val.parse(iter, json_end_pos);
                vtc = val;
            } break;
        default:
            break;
        }
        
        array_val_.push_back(vtc);
        if (iter != json_end_pos && *iter == ']') { // 有些解析玩就直接指向']'， 如果不退出在回到循环会因值自增错过
            break;
        }
    }
    if (iter == json_end_pos) {
        throw runtime_error(GET_MSG("Array need to surround by []"));
    }
    ++iter;
    return iter;
}
string 
JsonArray::generate(void)
{
    ostringstream ostr;
    ostr << "[";
    for (std::size_t i = 0; i < array_val_.size(); ++i) {
        if (i != 0) { // 每输出一个类型后跟一个','
            ostr << ",";
        }
        if (array_val_[i].json_value_type_ == JSON_ARRAY_TYPE ||
                array_val_[i].json_value_type_ == JSON_OBJECT_TYPE) {
            ostr <<  array_val_[i].generate();
        } else {
            if (array_val_[i].json_value_type_ == JSON_STRING_TYPE) {
                ostr <<  "\"" << array_val_[i].generate() << "\"";
            } else {
                ostr << array_val_[i].generate();
            }
        }
    }

    ostr <<  "]";
    return ostr.str();
}


int JsonArray::add(ValueTypeCast value)
{
    array_val_.push_back(value);

    return 1;
}

int
JsonArray::erase(JsonIndex index)
{
    auto remove_iter = array_val_.begin();
    // 移动迭代器index个距离
    advance(remove_iter, index);
    array_val_.erase(remove_iter);
    
    return 1;
}

ostream& operator<<(ostream &os, JsonArray &rhs)
{
    os << rhs.generate();

    return os;
}

ValueTypeCast& 
JsonArray::operator[](size_t key)
{
    if (key < 0 || key >= array_val_.size()) {
        throw runtime_error(GET_MSG("There is out of range in JsonArray with key(%s)", key));
    }

    return array_val_[key];
}

bool 
JsonArray::operator==(const JsonArray& rhs) const
{
    if (array_val_.size() != rhs.array_val_.size()) {
        return false;
    }

    for (std::size_t i = 0; i > array_val_.size(); ++i) {
        if (array_val_[i].json_array_value_ != rhs.array_val_[i].json_array_value_) {
            return false;
        }
        if (array_val_[i] != rhs.array_val_[i]) {
            return false;
        }
    }

    return true;
}
bool JsonArray::operator!=(const JsonArray& rhs) const
{
    return !(*this == rhs);
}

JsonArray& 
JsonArray::operator=(JsonArray rhs)
{
    array_val_ = rhs.array_val_;

    return *this;
}

/////////////////////////////////////////////////////////////////

ValueTypeCast::ValueTypeCast(void) {}

ValueTypeCast::ValueTypeCast(JsonBool value)
    : json_value_type_(JSON_BOOL_TYPE), json_bool_value_(value) {}
    
ValueTypeCast::ValueTypeCast(JsonNumber value)
    : json_value_type_(JSON_NUMBER_TYPE), json_number_value_(value) {}

ValueTypeCast::ValueTypeCast(JsonString value)
    : json_value_type_(JSON_STRING_TYPE), json_string_value_(value) {}

ValueTypeCast::ValueTypeCast(JsonObject value)
    : json_value_type_(JSON_OBJECT_TYPE), json_object_value_(value) {}

ValueTypeCast::ValueTypeCast(JsonArray value)
    : json_value_type_(JSON_ARRAY_TYPE), json_array_value_(value) {}

ValueTypeCast::ValueTypeCast(JsonNull value)
    : json_value_type_(JSON_NULL_TYPE), json_null_value_(value) {}

ValueTypeCast::ValueTypeCast(const ValueTypeCast &value)
    : json_value_type_(value.json_value_type_),
      json_array_value_(value.json_array_value_),
      json_object_value_(value.json_object_value_),
      json_string_value_(value.json_string_value_),
      json_number_value_(value.json_number_value_),
      json_bool_value_(value.json_bool_value_),
      json_null_value_(value.json_null_value_)
{}

ValueTypeCast::~ValueTypeCast(void) {}

ValueTypeCast::operator JsonBool()
{
    if (json_value_type_ == JSON_BOOL_TYPE) {
        return json_bool_value_;
    } else {
        throw runtime_error(GET_MSG("value cast faled: current type is not bool"));
    }
}

ValueTypeCast::operator JsonNumber()
{
    if (json_value_type_ == JSON_NUMBER_TYPE) {
        return json_number_value_;
    } else {
        throw runtime_error(GET_MSG("value cast faled: current type is not number"));
    }
}
ValueTypeCast::operator JsonString()
{
    if (json_value_type_ == JSON_STRING_TYPE) {
        return json_string_value_;
    } else {
        throw runtime_error(GET_MSG("value cast faled: current type is not string"));
    }
}

ValueTypeCast::operator JsonObject()
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_;
    } else {
        throw runtime_error(GET_MSG("value cast faled: current type is not object"));
    }
}
ValueTypeCast::operator JsonArray()
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_array_value_;
    } else {
        throw runtime_error(GET_MSG("value cast faled: current type is not array"));
    }
}

ValueTypeCast::operator JsonNull()
{
    if (json_value_type_ == JSON_NULL_TYPE) {
        return json_null_value_;
    } else {
        throw runtime_error(GET_MSG("value cast failed: current type is not null"));
    }
}

ValueTypeCast& 
ValueTypeCast::operator=(JsonBool val)
{
    json_value_type_ = JSON_BOOL_TYPE;
    json_bool_value_ = val;

    return *this;
}

ValueTypeCast& 
ValueTypeCast::operator=(JsonNumber val)
{
    json_value_type_ = JSON_NUMBER_TYPE;
    json_number_value_ = val;

    return *this;
}

ValueTypeCast& 
ValueTypeCast::operator=(JsonString val)
{
    json_value_type_ = JSON_STRING_TYPE;
    json_string_value_ = val;

    return *this;
}

ValueTypeCast& 
ValueTypeCast::operator=(JsonObject val)
{
    json_value_type_ = JSON_OBJECT_TYPE;
    json_object_value_ = val;

    return *this;
}

ValueTypeCast& 
ValueTypeCast::operator=(JsonArray val)
{
    json_value_type_ = JSON_ARRAY_TYPE;
    json_array_value_ = val;

    return *this;
}

ValueTypeCast& 
ValueTypeCast::operator=(JsonNull val)
{
    json_value_type_ = JSON_NULL_TYPE;
    json_null_value_ = val;

    return *this;
}

ValueTypeCast& 
ValueTypeCast::operator=(ValueTypeCast val)
{
    json_value_type_ = val.json_value_type_;
    json_bool_value_ = val.json_bool_value_;
    json_null_value_ = val.json_null_value_;
    json_array_value_ = val.json_array_value_;
    json_object_value_ = val.json_object_value_;
    json_string_value_ = val.json_string_value_;
    json_number_value_ = val.json_number_value_;

    return *this;
}

ostream& operator<<(ostream &os, ValueTypeCast &rhs)
{
    os << rhs.format_json();
    return os;
}

bool ValueTypeCast::operator==(const ValueTypeCast& rhs) const
{
    switch (json_value_type_)
    {
    case JSON_NULL_TYPE:
        return json_null_value_ == rhs.json_null_value_;
    case JSON_NUMBER_TYPE:
        return json_number_value_ == rhs.json_number_value_;
    case JSON_STRING_TYPE:
        return json_string_value_ == rhs.json_string_value_;
    case JSON_BOOL_TYPE:
        return json_bool_value_ == rhs.json_bool_value_;
    case JSON_ARRAY_TYPE:
        return json_array_value_ == rhs.json_array_value_;
    case  JSON_OBJECT_TYPE:
        return json_object_value_ == rhs.json_object_value_;
    default:
        break;
    }

    return true;
}
bool ValueTypeCast::operator!=(const ValueTypeCast& rhs) const
{
    return !(*this == rhs);
}

ValueTypeCast& ValueTypeCast::operator[](JsonIndex key)
{
    if (json_value_type_ == JSON_OBJECT_TYPE && 
            key.get_type() == JSON_STRING_TYPE) {
        return json_object_value_[key];
    } else if (json_value_type_ == JSON_ARRAY_TYPE && 
            key.get_type() == JSON_NUMBER_TYPE) {
        return json_array_value_[key];
    } else {
        throw runtime_error(GET_MSG("Json: Out of range"));
    }
}

ByteBuffer_Iterator 
ValueTypeCast::parse(ByteBuffer_Iterator &value_start_pos, ByteBuffer_Iterator &json_end_pos)
{
    VALUE_TYPE ret_type = this->check_value_type(value_start_pos);
    json_value_type_ = ret_type;
    switch (ret_type)
    {
    case JSON_NULL_TYPE:
        return json_null_value_.parse(value_start_pos, json_end_pos);
    case JSON_NUMBER_TYPE:
        return json_number_value_.parse(value_start_pos, json_end_pos);
    case JSON_STRING_TYPE:
        return json_string_value_.parse(value_start_pos, json_end_pos);
    case JSON_BOOL_TYPE:
        return json_bool_value_.parse(value_start_pos, json_end_pos);
    case JSON_ARRAY_TYPE:
        return json_array_value_.parse(value_start_pos, json_end_pos);
    case  JSON_OBJECT_TYPE:
        return json_object_value_.parse(value_start_pos, json_end_pos);
    default:
        string err_str = GET_MSG("Unknown json type (object or array)");
        break;
    }

    return json_end_pos;
}

string ValueTypeCast::generate(void)
{
    switch (json_value_type_)
    {
    case JSON_NULL_TYPE:
        return json_null_value_.generate();
    case JSON_NUMBER_TYPE:
        return json_number_value_.generate();
    case JSON_STRING_TYPE:
        return json_string_value_.generate();
    case JSON_BOOL_TYPE:
        return json_bool_value_.generate();
    case JSON_ARRAY_TYPE:
        return json_array_value_.generate();
    case  JSON_OBJECT_TYPE:
        return json_object_value_.generate();
    default:
        break;
    }

    return "";
}

string ValueTypeCast::format_json(void)
{
    if (json_value_type_ == JSON_OBJECT_TYPE || json_value_type_ == JSON_ARRAY_TYPE){
        string raw_json = this->generate();

        int tab = 0;
        ostringstream oformat_json;
        for (std::size_t i = 0; i < raw_json.size(); ++i) {
            if (raw_json[i] == '{' || raw_json[i] == '[') {
                ++tab;
                oformat_json << raw_json[i];
                oformat_json << '\n';
                for (int j = 0; j < tab; ++j) {
                    oformat_json << '\t';
                }
                continue;
            } else if (raw_json[i] == '}' || raw_json[i] == ']') {
                oformat_json << '\n';
                --tab;
                for (int j = 0; j < tab; ++j) {
                    oformat_json << '\t';
                }
                oformat_json << raw_json[i];
                continue;
            } else if (raw_json[i] == ',' && 
                            (raw_json[i+1] == '"' || 
                            raw_json[i+1] == '{' || 
                            raw_json[i+1] == '[')) {
                oformat_json << raw_json[i];
                oformat_json << '\n';
                for (int j = 0; j < tab; ++j) {
                    oformat_json << '\t';
                }
                continue;
            } else {
                oformat_json << raw_json[i];
            }
        }

        return oformat_json.str().c_str();
    } else {
        return this->generate();
    }
}

// 查找元素
JsonIter 
ValueTypeCast::find(const string &key)
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.find(key);
    }

    throw runtime_error(GET_MSG("Not support find function!"));
    return json_object_value_.object_val_.end();
}
// 操作元素
int ValueTypeCast::erase(JsonIndex key)
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.erase(key);
    } else if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.erase(key);
    }

    throw runtime_error(GET_MSG("Not support erase function!"));
    return -1;
}

// 当前类型为对象时添加元素
int ValueTypeCast::add(JsonIndex key, ValueTypeCast value)
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.add(key, value);
    } else if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.add(value);
    }

    throw runtime_error(GET_MSG("Not support add function!"));
    return -1;
}

int ValueTypeCast::add(JsonIndex key, JsonString value)
{
    JsonString val(value);
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.add(key, val);
    } else if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.add(val);
    }

    throw runtime_error(GET_MSG("Not support add function!"));
    return -1;
}

int ValueTypeCast::add(JsonIndex key, JsonBool value)
{
    JsonBool val(value);
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.add(key, val);
    } else if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.add(val);
    }

    throw runtime_error(GET_MSG("Not support add function!"));
    return -1;
}

int ValueTypeCast::add(JsonIndex key, JsonNumber value)
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.add(key, value);
    } else if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.add(value);
    }

    throw runtime_error(GET_MSG("Not support add function!"));
    return -1;
}


JsonIter 
ValueTypeCast::begin(void)
{
    if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.array_val_.begin();
    } else if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.object_val_.begin();
    } else {
        string err_str = GET_MSG("unknown type: call itertor end() failed!");
        throw runtime_error(err_str);
    }

    return json_object_value_.object_val_.begin();
}

JsonIter 
ValueTypeCast::end(void)
{
    if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.array_val_.end();
    } else if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.object_val_.end();
    } else {
        string err_str = GET_MSG("unknown type: call itertor end() failed!");
        throw runtime_error(err_str);
    }

    return json_object_value_.object_val_.end();
}


///////////////////////////////// WeJson //////////////////////////////////////////////////
WeJson::WeJson(void)
{

}
    
WeJson::WeJson(const string &json)
{
    this->parse(json);
}
    
WeJson::WeJson(ByteBuffer &data)
{
    this->parse(data);
}

WeJson::~WeJson(void)
{
    
}


int 
WeJson::parse(ByteBuffer &buff)
{
    ByteBuffer simple_json_text;
    ByteBuffer_Iterator start_pos;
    
    // 找json文本的开始，如果遇到不是'{'开始的会返回失败
    for (start_pos = buff.begin(); start_pos != buff.end(); ++start_pos) {
        VALUE_TYPE ret = JsonType::check_value_type(start_pos);
        if (ret == JSON_OBJECT_TYPE || ret == JSON_ARRAY_TYPE) {
            break;
        }
    }

    if (start_pos == buff.end()) {
        throw runtime_error(GET_MSG("Can't find any json text!"));
    }

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

        int i = 0;
        for (i = 0; i < 4; ++i) {
            if (sperate_chars[i] == *iter) {
                break;
            }
        }
        // 如果匹配到json的结构字符并且不是在字符串中，那么就忽略
        if (i < 4 && quotation_marks == false) {
            continue;
        }

        // "//" 以这个开头的注释也会被忽略，这个注释可以单独一行或是跟在json那行的末尾，不能出现在json文本那行的前面
        if (quotation_marks == false && *iter == '/' && *(iter+1) == '/') {
            for (; iter != buff.end(); ++iter) {
                if (*iter == '\n') {
                    break;
                }
            }

            if (*iter == '\n') {
                continue;
            }
        }

        simple_json_text.write_int8(*iter);
    }
    auto begin_json = simple_json_text.begin();
    auto end_json = simple_json_text.end();

    ValueTypeCast::parse(begin_json, end_json);

    return 0;
}

int 
WeJson::parse(const string &json)
{
    ByteBuffer buff;
    buff.write_string(json);
    this->parse(buff);

    return 0;
}

string 
WeJson::generate(void)
{
    return ValueTypeCast::generate();
}

string 
WeJson::format_json(void)
{
    return ValueTypeCast::format_json();
}

}
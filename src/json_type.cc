#include "json_type.h"

namespace my_util {

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
        string err_str = get_msg("unknown type: call JsonIter::operator++(void) failed!");
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
        string err_str = get_msg("unknown type: call JsonIter::operator++(int) failed!");
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
        string err_str = get_msg("unknown type: call JsonIter::operator--(void) failed!");
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
        string err_str = get_msg("unknown type: call JsonIter::operator--(int) failed!");
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
        string err_str = get_msg("unknown type: call JsonIter::operator*() failed!");
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
        string err_str = get_msg("unknown type: call JsonIter::operator*() failed!");
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

string 
JsonType::get_json_text(ByteBuffer_Iterator &value_curr_pos, int range)
{
    ostringstream ostr;
    ostr << "json text arround error: " << endl;
    auto lower_limit = value_curr_pos - range > value_curr_pos.begin() ? value_curr_pos - range : value_curr_pos.begin();
    auto upper_limit = value_curr_pos + range >= value_curr_pos.end() ? value_curr_pos.end()-1 : value_curr_pos + range;

    auto pos = lower_limit;
    for (; pos <= upper_limit; ++pos) {
        ostr << *pos;
    }

    if (value_curr_pos != value_curr_pos.end()) {
        ostr << endl << " Error occur pos: " << *value_curr_pos;
    } else {
        ostr << endl << " Error occur pos: At End";
    }
    return ostr.str();
}

string
JsonType::debug_info(ByteBuffer_Iterator &value_curr_pos)
{
    return this->get_json_text(value_curr_pos, 30); //先定60个字符的范围 
}

///////////////////////////////////////////////////////////

JsonNumber::JsonNumber(void)
: value_type_(NONE_TYPE),
  double_value_(0),
  int_value_(0)
  
{

}
JsonNumber::JsonNumber(double val)
: value_type_(DOUBLE_TYPE), 
  double_value_(val),
  int_value_(0)
{
}
JsonNumber::JsonNumber(int32_t val)
: value_type_(INT32_TYPE), 
  double_value_(0),
  int_value_(val)
{

}

JsonNumber::JsonNumber(const JsonNumber& val)
: value_type_(val.value_type_), 
  double_value_(val.double_value_),
  int_value_(val.int_value_)
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
            string err_str = get_msg("Zero can't be first number of integer!\n%s", this->debug_info(iter).c_str());
            throw runtime_error(err_str);
        }
    }

    bool is_decimal = false;
    for (; iter != json_end_pos && *iter != ','; ++iter) {
        if (!isdigit(*iter) && *iter != '.') {
            break;
        }

        if (*iter == '.') {
            is_decimal = true;
        }
        str += *iter;
    }
    if (is_decimal) {
        value_type_ = DOUBLE_TYPE;
        double_value_ = stod(str) * (is_negative?-1:1);
    } else {
        value_type_ = INT32_TYPE;
        int_value_ = stoi(str) * (is_negative?-1:1);
    }

    return iter;
}

string 
JsonNumber::generate(void) 
{
    ostringstream os;
    switch (value_type_)
    {
    case DOUBLE_TYPE:
    {
        os << double_value_;
    } break;
    case INT32_TYPE:
    {
        os << int_value_;
    } break;
    default:
        break;
    }

    return os.str();
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
    if (value_type_ == rhs.value_type_) {
        if (value_type_ == INT32_TYPE) {
            return int_value_ == rhs.int_value_?true:false;
        } else if (value_type_ == DOUBLE_TYPE) {
            return double_value_ == rhs.double_value_?true:false;
        }
    } else {
        if (value_type_ == INT32_TYPE) {
            double tmp = int_value_;
            return (tmp == rhs.double_value_? true: false); 
        }
        else {
            double tmp = rhs.int_value_;
            return (tmp == double_value_? true: false); 
        }
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
    value_type_ = rhs.value_type_;
    if (value_type_ == INT32_TYPE) {
        int_value_ = rhs.int_value_;
    } else if (value_type_ == DOUBLE_TYPE) {
        double_value_ = rhs.double_value_;
    }

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
        string err_str = get_msg("Expected true or false, but result is %s\n%s", str.c_str(), this->debug_info(iter).c_str());
        throw runtime_error(err_str);
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
        string err_str = get_msg("Expected null, but result is %s\n%s", str.c_str(), this->debug_info(iter).c_str());
        throw runtime_error(err_str);
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
        string err_str = get_msg("String need to surround by \"\" \n%s", this->debug_info(iter).c_str());
        throw runtime_error(err_str);
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
                string err_str = get_msg("Unknown character in object: %c\n%s", *iter, this->debug_info(iter).c_str());
                throw runtime_error(err_str);
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
            string err_str = get_msg("There's already \"%s\" exists.\n%s", this->debug_info(iter).c_str());
            throw runtime_error(err_str);
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
        string err_str = get_msg("Can't find value in JsonObject with key(%s)", key.c_str());
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
                string err_str = get_msg("Unknown character in array: %c\n%s", *iter, this->debug_info(iter).c_str());
                throw runtime_error(err_str);
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
        string err_str = get_msg("Array need to surround by []\n%s", this->debug_info(iter).c_str());
        throw runtime_error(err_str);
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
        string err_str = get_msg("There is out of range in JsonArray with key(%s)", key);
        throw runtime_error(err_str);
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
        string err_str = get_msg("value cast faled: current type is not bool");
        throw runtime_error(err_str);
    }
}

ValueTypeCast::operator JsonNumber()
{
    if (json_value_type_ == JSON_NUMBER_TYPE) {
        return json_number_value_;
    } else {
        string err_str = get_msg("value cast faled: current type is not number");
        throw runtime_error(err_str);
    }
}
ValueTypeCast::operator JsonString()
{
    if (json_value_type_ == JSON_STRING_TYPE) {
        return json_string_value_;
    } else {
        string err_str = get_msg("value cast faled: current type is not string");
        throw runtime_error(err_str);
    }
}

ValueTypeCast::operator JsonObject()
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_;
    } else {
        string err_str = get_msg("value cast faled: current type is not object");
        throw runtime_error(err_str);
    }
}
ValueTypeCast::operator JsonArray()
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_array_value_;
    } else {
        string err_str = get_msg("value cast faled: current type is not array");
        throw runtime_error(err_str);
    }
}

ValueTypeCast::operator JsonNull()
{
    if (json_value_type_ == JSON_NULL_TYPE) {
        return json_null_value_;
    } else {
        string err_str = get_msg("value cast failed: current type is not null");
        throw runtime_error(err_str);
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
        string err_str = get_msg("Json: Out of range");
        throw runtime_error(err_str);
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
        string err_str = get_msg("Unknown json type (object or array)");
        throw runtime_error(err_str);
        break;
    }
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

    throw runtime_error(get_msg("Not support find function!"));
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

    throw runtime_error(get_msg("Not support erase function!"));
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

    throw runtime_error(get_msg("Not support add function!"));
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

    throw runtime_error(get_msg("Not support add function!"));
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

    throw runtime_error(get_msg("Not support add function!"));
    return -1;
}

int ValueTypeCast::add(JsonIndex key, JsonNumber value)
{
    if (json_value_type_ == JSON_OBJECT_TYPE) {
        return json_object_value_.add(key, value);
    } else if (json_value_type_ == JSON_ARRAY_TYPE) {
        return json_array_value_.add(value);
    }

    throw runtime_error(get_msg("Not support add function!"));
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
        string err_str = get_msg("unknown type: call itertor end() failed!");
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
        string err_str = get_msg("unknown type: call itertor end() failed!");
        throw runtime_error(err_str);
    }

    return json_object_value_.object_val_.end();
}

}
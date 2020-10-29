#ifndef __WEJSON_H__
#define __WEJSON_H__

#include "basic_head.h"
#include "byte_buffer.h"
#include "file.h"
#include "json_type.h"

namespace my_util {

class WeJson : public ValueTypeCast {
public:
    WeJson(void);
    WeJson(string str);
    ~WeJson(void);

    // 从文件中读取json文本
    int open_json(string json_file_path);
    // 将json写入到文件中
    int write_json(string json_file_path);
    // 解析字符串中保存的json文本
    int parser_from_json(string str);
    // 解析ByteBuffer保存的json文本
    int parser_from_json(ByteBuffer &buff);

    // 当出错时输出调试信息
    string debug_info(void);

public:
    // json中重要的分割字符
    const vector<char> sperate_chars = {' ', '\r', '\n','\t','{', '}','[', ']',',',':','"'};
};

///////////////////// JSON 迭代器////////////////////////////////
// class WeJson_Iterator : public iterator<random_access_iterator_tag, int8_t> 
// {
//     friend class WeJson;
// public:
//     WeJson_Iterator(void) = default;
//     explicit WeJson_Iterator(const WeJson *buff)
//             : buff_(buff), curr_pos_(){}

//     WeJson_Iterator begin() 
//     {
//         WeJson 
//         return tmp;
//     }

//     WeJson_Iterator end()
//     {
//         WeJson_Iterator tmp = *this;
//         tmp.curr_pos_ = buff_->start_write_pos_;
//         return tmp;
//     }

//     int8_t operator*()
//     {
//         if (curr_pos_ == buff_->start_write_pos_) {
//             ostringstream ostr;
//             ostr << "Line: " << __LINE__ << " WeJson_Iterator operator+ out of range.";
//             ostr << "debug_info: " << this->debug_info() << std::endl;
//             throw runtime_error(ostr.str());
//         }
//         return buff_->buffer_[curr_pos_];
//     }
//     // 前置++
//     WeJson_Iterator& operator++()
//     {
//         if (curr_pos_ == buff_->start_write_pos_)
//         {
//             return *this;
//         }
//         curr_pos_ = (curr_pos_ + buff_->max_buffer_size_ + 1) % buff_->max_buffer_size_;
//         return *this;
//     }
//     // 后置++
//     WeJson_Iterator& operator++(int)
//     {
//         if (curr_pos_ == buff_->start_write_pos_)
//         {
//             return *this;
//         }

//         WeJson_Iterator &tmp = *this;
//         curr_pos_ = (curr_pos_ + buff_->max_buffer_size_ + 1) % buff_->max_buffer_size_;

//         return tmp;
//     }
//     // 前置--
//     WeJson_Iterator& operator--()
//     {
//         if (curr_pos_ == buff_->start_read_pos_) {
//             curr_pos_ = buff_->start_write_pos_; // 起始位置都是不可再减少的位置
//             return *this;
//         }

//         curr_pos_ = (curr_pos_ + buff_->max_buffer_size_ - 1) % buff_->max_buffer_size_;

//         return *this;
//     }
//     // 后置--
//     WeJson_Iterator& operator--(int)
//     {
//         if (curr_pos_ == buff_->start_read_pos_) {
//             curr_pos_ = buff_->start_write_pos_;
//             return *this;
//         }

//         WeJson_Iterator &tmp = *this;
//         curr_pos_ = (curr_pos_ + buff_->max_buffer_size_ - 1) % buff_->max_buffer_size_;

//         return tmp;
//     }
//     WeJson_Iterator operator+(int inc)
//     {
//         WeJson_Iterator tmp_iter = *this;
//         for (int i = 0; i < inc; ++i) {
//             ++tmp_iter;
//             if (tmp_iter.curr_pos_ == tmp_iter.buff_->start_write_pos_) {
//                 break;
//             }
//         }

//         return tmp_iter;
//     }

//     WeJson_Iterator operator-(int inc) {
//         WeJson_Iterator tmp_iter = *this;
//         for (int i = 0; i < inc; ++i) {
//             --tmp_iter;
//             if (tmp_iter.curr_pos_ == tmp_iter.buff_->start_read_pos_) {
//                 break;
//             }
//         }

//         return tmp_iter;
//     }
//     // 只支持 == ,!= , = 其他的比较都不支持
//     bool operator==(const WeJson_Iterator& iter) const {
//         if (this->value_type_ != iter.value_type_) {
//             return false;
//         }

//         if (this->buff_ != iter.buff_) {
//             return false;
//         }

//         if (this->value_type_ == JSON_OBJECT_TYPE) {
//             if (this->curr_object_iter_ == iter.curr_object_iter_) {
//                 return true;
//             }
//         }

//         if (this->value_type_ == JSON_ARRAY_TYPE) {
//             if (this->curr_array_iter_ == iter.curr_array_iter_) {
//                 return true;
//             }
//         }

//         return false;
//     }

//     bool operator!=(const WeJson_Iterator& iter) const {
//         return !(*this == iter);
//     }
   
//     WeJson_Iterator& operator=(const WeJson_Iterator& src)
//     {
//         if (src != *this) {
//             buff_ = src.buff_;
//         }

//         return *this;
//     }

// private:
//     const WeJson *buff_ = nullptr;
//     VALUE_TYPE value_type_;
//     map<string, ValueTypeCast>::iterator curr_object_iter_;
//     vector<ValueTypeCast>::iterator curr_array_iter_;
// };

}

#endif
#ifndef __DATA_STRUCTURE__
#define __DATA_STRUCTURE__

#include "basic_head.h"
#include "msg_record.h"

namespace my_utils {

// 设置所有系统函数的回调输出
extern msg_to_stream_callback g_msg_to_stream_trace;
extern msg_to_stream_callback g_msg_to_stream_debug;
extern msg_to_stream_callback g_msg_to_stream_info;
extern msg_to_stream_callback g_msg_to_stream_warn;
extern msg_to_stream_callback g_msg_to_stream_error;
extern msg_to_stream_callback g_msg_to_stream_fatal;

extern void set_datastruct_message_output_callback(InfoLevel level, msg_to_stream_callback func);

/////////////////////////// 数据结构库基本元素 ///////////////////////////////////
template <class T>
struct Element {
    T data;

    Element *pre_node;
    Element *next_node;

    Element(void);
    virtual ~Element(void);
};


template<class T>
Element<T>::Element(void)
: pre_node(nullptr), next_node(nullptr)
{}

template<class T>
Element<T>::~Element(void)
{}

/////////////////////////// 队列 ////////////////////////////////////////////////

}

#endif
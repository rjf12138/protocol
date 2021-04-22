#ifndef __BASIC_HEAD_H__
#define __BASIC_HEAD_H__
/////////////// c++ header file //////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdarg>
#include <map>
#include <memory>
#include <queue>
#include <utility>

#include <csignal>

using namespace std;

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__)  // 确定当前运行的系统
    #define __RJF_WINDOWS__
#elif defined(__gnu_linux__) || defined(__linux__)
    #define __RJF_LINUX__
#endif

#if defined(__RJF_LINUX__)
////////////// linux system header file ////////////////
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <setjmp.h>

#elif defined(__RJF_WINDOWS__)

#endif

//////////////////////////// 
typedef char            Char;
typedef unsigned char   UChar;
typedef int8_t          Int8;
typedef uint8_t         UInt8;
typedef int16_t         Int16;
typedef uint16_t        UInt16;
typedef int32_t         Int32;
typedef uint32_t        UInt32;
typedef int64_t         Int64;
typedef uint64_t        UInt64;
typedef std::size_t     StdSize;
typedef size_t          Size_t;

#endif

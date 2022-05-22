cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(protocol)

set(CMAKE_DEBUG_POSTFIX d)

set(CXX_FLAGS
    -g
    # -DVALGRIND
    -DCHECK_PTHREAD_RETURN_VALUE
    -D_FILE_OFFSET_BITS=64
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wno-unused-parameter
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
    -march=native
    # -MMD
    -rdynamic
    )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

# 设置生成文件输出路径
set(LIBRARY_OUTPUT_PATH, ./output/debug/lib)
add_library(protocol STATIC "")

target_include_directories(protocol PUBLIC ./inc/)
target_include_directories(protocol PRIVATE ./extern_inc/)

target_link_directories(protocol PRIVATE ./lib/debug/)

target_link_libraries(protocol PRIVATE basicd)
target_link_libraries(protocol PRIVATE algorithmd)
target_link_libraries(protocol PRIVATE gtest)
target_link_libraries(protocol PRIVATE pthread)
include(src/CMakeLists.txt)


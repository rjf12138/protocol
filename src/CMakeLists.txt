project(protocol)

set(LIBRARY_OUTPUT_PATH /home/ruanjian/workspace/project_pre/protocol/output/debug/lib)

aux_source_directory(. DIR_LIB_SRCS)

add_library(protocol ${DIR_LIB_SRCS})

target_link_libraries(protocol -lbuffer)
target_link_libraries(protocol -lpthread)
target_link_libraries(protocol -lmessage)


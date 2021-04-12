project(protocol)

set(LIBRARY_OUTPUT_PATH /home/ruanjian/workspace/project_pre/protocol/output/debug/lib)

aux_source_directory(. DIR_LIB_SRCS)

add_library(json ${DIR_LIB_SRCS})


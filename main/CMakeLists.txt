
project(protocol)

set(EXECUTABLE_OUTPUT_PATH /home/ruanjian/workspace/project_pre/protocol/output/debug/bin)

add_executable(http_test http_test.cc)

target_link_libraries(http_test -lprotocol)
target_link_libraries(http_test -lhttp)
target_link_libraries(http_test -ljson)
target_link_libraries(http_test -lgmock)
target_link_libraries(http_test -lgmock_main)
target_link_libraries(http_test -lgtest)
target_link_libraries(http_test -lgtest_main)
target_link_libraries(http_test -lbuffer)
target_link_libraries(http_test -lpthread)
target_link_libraries(http_test -lmessage)
target_link_libraries(http_test -lsystem_util)


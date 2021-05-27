
project(protocol)

set(EXECUTABLE_OUTPUT_PATH /home/ruanjian/workspace/project_pre/protocol/output/debug/bin)

add_executable(WeJson_Test WeJson_Test.cc)

target_link_libraries(WeJson_Test -lprotocol)
target_link_libraries(WeJson_Test -lgmock)
target_link_libraries(WeJson_Test -lgmock_main)
target_link_libraries(WeJson_Test -lgtest)
target_link_libraries(WeJson_Test -lgtest_main)
target_link_libraries(WeJson_Test -lbuffer)
target_link_libraries(WeJson_Test -lpthread)
target_link_libraries(WeJson_Test -lmessage)


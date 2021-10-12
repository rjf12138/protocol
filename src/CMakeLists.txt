project(protocol)

target_include_directories(protocol PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_sources(protocol PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/./url_parser.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/./websocket.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/./http.cc)

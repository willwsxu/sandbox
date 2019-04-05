cmake_minimum_required(VERSION 3.12.1)
project(time_test LANGUAGES CXX)
set( CMAKE_VERBOSE_MAKEFILE on )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(test_time_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/time_test.cpp
	)
add_executable(time_test ${test_time_SOURCE_FILES})
add_subdirectory(${PROJECT_SOURCE_DIR}/proto_test/addressbook_producer)
add_subdirectory(${PROJECT_SOURCE_DIR}/proto_test/addressbook_consumer)
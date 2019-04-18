cmake_minimum_required(VERSION 3.12.1)
project(ttstl LANGUAGES CXX)

set(ttstl_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(ttstl_HEADER_FILES
    ${ttstl_INCLUDE_DIR}/ttstl/custom_allocator.h
    ${ttstl_INCLUDE_DIR}/ttstl/deferred.h
    ${ttstl_INCLUDE_DIR}/ttstl/event.h
    ${ttstl_INCLUDE_DIR}/ttstl/exclusive_while.h
    ${ttstl_INCLUDE_DIR}/ttstl/exponential_backoff.h
    ${ttstl_INCLUDE_DIR}/ttstl/hash.h
    ${ttstl_INCLUDE_DIR}/ttstl/likely.h
    ${ttstl_INCLUDE_DIR}/ttstl/list_to_string.h
    ${ttstl_INCLUDE_DIR}/ttstl/locked_queue.h
    ${ttstl_INCLUDE_DIR}/ttstl/observable.h
    ${ttstl_INCLUDE_DIR}/ttstl/observable_impl.h
    ${ttstl_INCLUDE_DIR}/ttstl/observable_pimpl.h
    ${ttstl_INCLUDE_DIR}/ttstl/platform.h
    ${ttstl_INCLUDE_DIR}/ttstl/read_write_locks.h
    ${ttstl_INCLUDE_DIR}/ttstl/ring_buffer.h
    ${ttstl_INCLUDE_DIR}/ttstl/sha.h
    ${ttstl_INCLUDE_DIR}/ttstl/spin_lock.h
    ${ttstl_INCLUDE_DIR}/ttstl/sstream.h
    ${ttstl_INCLUDE_DIR}/ttstl/string.h
    ${ttstl_INCLUDE_DIR}/ttstl/stringbuf.h
    ${ttstl_INCLUDE_DIR}/ttstl/task_queue.h
    ${ttstl_INCLUDE_DIR}/ttstl/thread_name.h
    ${ttstl_INCLUDE_DIR}/ttstl/ticket_lock.h
    ${ttstl_INCLUDE_DIR}/ttstl/uuid.h
    ${ttstl_INCLUDE_DIR}/ttstl/uuid_hash.h
    ${ttstl_INCLUDE_DIR}/ttstl/vector_set.h
    ${ttstl_INCLUDE_DIR}/ttstl/waitable_queue.h)

add_library(ttstl INTERFACE)
#add_library(the_arsenal::ttstl ALIAS ttstl)
target_compile_features(ttstl INTERFACE cxx_std_11)
target_compile_definitions(ttstl INTERFACE $<$<PLATFORM_ID:Windows>:NOMINMAX>)
target_include_directories(ttstl INTERFACE $<BUILD_INTERFACE:${ttstl_INCLUDE_DIR}>)

if(debesys_CREATE_UNIT_TESTS)
    message(STATUS "Adding project to testbed: ${PROJECT_NAME}")
    set(ttstl_test_SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/test/gtest/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/test/gtest/ttstring_test.cpp)
    add_executable (ttstl_test ${ttstl_test_SOURCE_FILES})
    target_link_libraries(ttstl_test
                          PRIVATE the_arsenal::ttstl
                          PRIVATE GTest::GTest GTest::Main)

    add_test(StringTest ttstl_test)
endif()

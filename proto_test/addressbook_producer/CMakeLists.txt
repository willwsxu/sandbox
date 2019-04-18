cmake_minimum_required(VERSION 3.12.1)
project(proto_producer LANGUAGES CXX)
set( CMAKE_VERBOSE_MAKEFILE on )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(all_messages_GENERATED_DIR ${CMAKE_CURRENT_LIST_DIR}/source/generated)

if (WIN32)

#set(Use_Proto261 YES)
if (Use_Proto261)
set(proto261_root "C:/repo-tt/protobuf/protobuf-2.6.1")
set(Protobuf_PROTOC_EXECUTABLE "${proto261_root}/bin/protoc.exe")
set(Protobuf_INCLUDE_DIR "${proto261_root}/src")
set(Protobuf_INCLUDE_DIRS "${proto261_root}/src")
set(Protobuf_LIBRARIES "${proto261_root}/bin/x64/debug/libprotobuf.lib")
#set(Protobuf_LIBRARY "${proto261_root}/bin/x64/debug/libprotobuf.lib")
endif()

else()
set(linux_subsystem_root "/mnt/c/repo-tt/debesys/the_arsenal/all_messages/protobuf-2.6.1")
set(linux_subsystem_root_bin "${linux_subsystem_root}/compilers/centos-6/")
set(Protobuf_PROTOC_EXECUTABLE "${linux_subsystem_root_bin}/bin/protoc")
set(Protobuf_INCLUDE_DIR "${linux_subsystem_root}/cpp/include")
set(Protobuf_INCLUDE_DIRS "${Protobuf_INCLUDE_DIR}")
set(Protobuf_LIBRARIES "${linux_subsystem_root_bin}/lib/libprotobuf.a")
endif()

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf REQUIRED)
#set(Protobuf_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/source/tt/messaging")
message("   --> PROTOBUF LIB: ${Protobuf_LIBRARY}")
message("   --> PROTOBUF exe: ${Protobuf_PROTOC_EXECUTABLE}")
message("   --> PROTOBUF INCLUDE: ${Protobuf_INCLUDE_DIRS}")
message("   --> PROTOBUF VERSION: ${Protobuf_VERSION}")
message("   --> PROTOBUF Found: ${Protobuf_FOUND}")

message(STATUS "producer TT 2 Discovered Protobuf ${Protobuf_VERSION}")
message(STATUS "Using protoc ${Protobuf_PROTOC_EXECUTABLE}")

set(PROTO_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/source)
file(GLOB_RECURSE all_messages_PROTOS "${CMAKE_CURRENT_LIST_DIR}/source/*.proto")
foreach(PROTO_FILE ${all_messages_PROTOS})
	message(STATUS "Generating ${PROTO_FILE}")
    set(PROTOC_COMMAND_ARGS
        "--cpp_out=${all_messages_GENERATED_DIR}"
        "--proto_path=${PROTO_SOURCE_DIR}"
        "${PROTO_FILE}")
    execute_process(COMMAND ${Protobuf_PROTOC_EXECUTABLE} ${PROTOC_COMMAND_ARGS}
                    RESULT_VARIABLE CMD_RESULT
                    ERROR_VARIABLE CMD_ERROR
                    WORKING_DIRECTORY ${PROTO_SOURCE_DIR})
    if(CMD_RESULT)
        message(FATAL_ERROR "PROTOC failed. error=${CMD_ERROR}")
    endif()
endforeach()

file(GLOB_RECURSE GENERATED_HEADERS "${all_messages_GENERATED_DIR}/*.h")
file(GLOB_RECURSE GENERATED_SOURCES "${all_messages_GENERATED_DIR}/*.cc")

set(proto_test_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/source/proto_test.cpp ${GENERATED_SOURCES}
	)
	
include_directories(proto_producer
                           PUBLIC $<BUILD_INTERFACE:${PROTOBUF_INCLUDE_DIRS}>
                           PUBLIC $<BUILD_INTERFACE:${all_messages_GENERATED_DIR}>
						   )
					  
add_executable(proto_producer ${proto_test_SOURCE_FILES}
				)
target_link_libraries(proto_producer
					PRIVATE ${Protobuf_LIBRARIES}
					PRIVATE ttstl
					)

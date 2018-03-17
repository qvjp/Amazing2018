cmake_minimum_required (VERSION 2.6)
project (amzjson_test C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

add_library(amzjson amzjson.c)
add_executable(amzjson_test test.c)
target_link_libraries(amzjson_test amzjson)

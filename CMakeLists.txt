cmake_minimum_required(VERSION 3.20)
project(Ip-of-Domain)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Ip-of-Domain main.cpp)

if(WIN32)
    target_link_libraries(Ip-of-Domain PRIVATE ws2_32)
endif()

if(MSVC)
    target_compile_options(Ip-of-Domain PRIVATE /utf-8)
endif()

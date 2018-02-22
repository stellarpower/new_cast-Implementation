cmake_minimum_required(VERSION 2.8)

project(Testsuite)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    string(SUBSTRING "${CMAKE_CXX_COMPILER_VERSION}" 0 1 GV)
    if ("${GV}" LESS "6")
        message("Warning - certain features require C++17 support, mostly fold expressions. Please upgrade to g++ > 6 to add support")
    endif()
endif()



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Global)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SharedCode)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SharedLibrarySideTests)

add_subdirectory(SharedLibrarySideTests)
add_subdirectory(SharedCode)
add_subdirectory(Application)


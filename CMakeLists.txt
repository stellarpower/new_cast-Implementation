cmake_minimum_required(VERSION 2.8)

project(NewCast CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -fmax-errors=63 ")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libstdc++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /showIncludes")
endif()


# ABI Type

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(ItaniumABI "TRUE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCOMPILER_GNU")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(MSVCABI "TRUE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCOMPILER_MSVC")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # set(MSVCABI "TRUE")
    set(ItaniumABI "TRUE")
    # set(DualABI "TRUE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCOMPILER_CLANG")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERBOSITY=2")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Implementation)
add_subdirectory(Testsuite)


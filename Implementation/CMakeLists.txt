
add_library(NewCast SHARED
    ../NewCast.h
    NewCast.cpp
)

message (STATUS "ABI ${CMAKE_CXX_COMPILER_ABI}")

if ("${ItaniumABI}" STREQUAL "TRUE")
    message (STATUS "Building Itanium Implementation - This library is covered under the terms of the GPLv3 with runtime exception")
    add_subdirectory(NewCastItanium)
    target_link_libraries(NewCast NewCastItanium)
endif()

if ("${MSVCABI}" STREQUAL "TRUE")
    message (STATUS "Building MSVC Implementation - This library is covered under the terms of the GPLv2.1 or later")
    add_subdirectory(NewCastMSVC)
    target_link_libraries(NewCast NewCastMSVC)
endif()


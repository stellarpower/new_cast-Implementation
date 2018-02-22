add_library(SharedLibrarySideTests SHARED
    SharedLibrarySideTests.cpp
    SharedLibrarySideTests.h
)

target_link_libraries(SharedLibrarySideTests SharedCode)


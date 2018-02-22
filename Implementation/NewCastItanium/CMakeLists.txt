
if(WIN32)
    add_library(NewCastItanium SHARED
        NewCastItanium.cpp
    	NewCastItanium.h
        ../../NewCast.h
        PolymorphicPropertiesWindows.cpp
    )
elseif(UNIX)
    add_library(NewCastItanium SHARED
        NewCastItanium.cpp
    	NewCastItanium.h
        ../../NewCast.h
        PolymorphicPropertiesUnix.cpp
    )
endif()

# Add header Files.


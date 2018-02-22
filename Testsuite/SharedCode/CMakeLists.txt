add_library(SharedCode STATIC
	Object.cpp
	ObjectManager.cpp
	TestsShared.cpp	TestsShared.h
        ../../GlobalFunctions.h
        ../Global/Object.h
        ../Global/ObjectManager.h
)


target_link_libraries(SharedCode NewCast)


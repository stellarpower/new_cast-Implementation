add_executable(Application
	Tests.h
	Tests.cpp
	main.cpp
)

target_link_libraries(Application SharedCode)
target_link_libraries(Application SharedLibrarySideTests)
target_link_libraries(Application NewCast)


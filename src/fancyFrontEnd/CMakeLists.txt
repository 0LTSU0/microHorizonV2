set(srcs
	fancyFrontEnd.h
	fancyFrontEnd.cpp)

add_library(fancyFrontEnd ${srcs})
target_include_directories(fancyFrontEnd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(fancyFrontEnd PUBLIC Shared)

target_link_libraries(fancyFrontEnd PRIVATE sfml-graphics sfml-window sfml-system)
target_include_directories(fancyFrontEnd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/SFML-3.0.1/include)

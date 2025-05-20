add_library(Shared INTERFACE)

target_link_libraries(Shared INTERFACE zlibstatic expat Tracer)

target_include_directories(Shared INTERFACE 
						   ${CMAKE_CURRENT_SOURCE_DIR}
						   "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/libosmium-2.20.0/include"
						   "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/protozero-1.7.1/include")
#target_include_directories(Shared PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/libosmium-2.20.0/include")
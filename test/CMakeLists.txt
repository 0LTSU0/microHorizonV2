configure_file(map_paths.h.in ${CMAKE_CURRENT_BINARY_DIR}/map_paths.h)

add_executable(test_microHorizon
	test_horizonWorker.cpp
	${CMAKE_CURRENT_BINARY_DIR}/map_paths.h)

target_include_directories(test_microHorizon PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(test_microHorizon
	gtest
	gtest_main
	horizonWorker)
	

add_executable(test_horizonWorker test_horizonWorker.cpp)

target_link_libraries(test_horizonWorker
	gtest
	gtest_main
	horizonWorker)
	

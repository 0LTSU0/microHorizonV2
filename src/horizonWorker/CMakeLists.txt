set(horizonGenSources
	horizonWorker.h
	horizonWorker.cpp
	posMatcherImpl.h
	posMatcherImpl.cpp)

add_library(horizonWorker ${horizonGenSources})
target_include_directories(horizonWorker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(horizonWorker PUBLIC Shared)
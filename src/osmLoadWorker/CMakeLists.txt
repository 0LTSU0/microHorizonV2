add_library(osmLoadWorker RoadLoader.cpp RoadLoader.h)
target_include_directories(osmLoadWorker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(osmLoadWorker PUBLIC Shared)
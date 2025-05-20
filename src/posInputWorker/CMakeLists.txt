add_library(posInputWorker posInputWorker.cpp)
target_include_directories(posInputWorker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(posInputWorker PUBLIC Shared)
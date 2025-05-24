cmake_minimum_required(VERSION 3.10)
project(microHorizon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(ThirdParty)
add_subdirectory(src)
add_subdirectory(test)

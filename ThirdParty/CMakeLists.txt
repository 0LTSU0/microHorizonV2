set(EXPAT_SHARED_LIBS OFF)

add_subdirectory(expat-2.6.3) #needed by osmium
add_subdirectory(zlib-1.3.1) #needed by osmium

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest-1.17.0)

set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
add_subdirectory(SFML-3.0.1)

set(EXPAT_SHARED_LIBS OFF)

add_subdirectory(expat-2.6.3) #needed by osmium
add_subdirectory(zlib-1.3.1) #needed by osmium

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest-1.17.0)
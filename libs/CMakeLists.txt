SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()

option(CPPZMQ_BUILD_TESTS "Whether or not to build the tests" OFF)
option(BUILD_EXAMPLES "Build the examples" OFF)
option(BUILD_TESTS "Build the tests" OFF)
set(BUILD_SHARED_LIBS FALSE)
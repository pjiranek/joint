CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (Joint)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES (${Joint_SOURCE_DIR}/src)

ENABLE_TESTING ()
FIND_PACKAGE (Gtest)
IF (GTEST_FOUND)
    MESSAGE ("Google Test found: performing tests")
    ADD_SUBDIRECTORY (test)
ELSE ()
    MESSAGE ("Google Test not found: no tests to be done")
ENDIF ()

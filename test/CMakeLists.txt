FIND_PACKAGE (Boost REQUIRED)

INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (../src)
INCLUDE_DIRECTORIES (${GTEST_INCLUDE_DIRS})
LINK_LIBRARIES (${GTEST_BOTH_LIBRARIES})

ADD_CUSTOM_TARGET (Test ALL COMMAND ctest -VV)

ADD_EXECUTABLE (TestIterator TestIterator.cpp)
ADD_TEST (NAME TestIterator COMMAND TestIterator)
ADD_DEPENDENCIES (Test TestIterator)

ADD_EXECUTABLE (TestAlgorithm TestAlgorithm.cpp)
ADD_TEST (NAME TestAlgorithm COMMAND TestAlgorithm)
ADD_DEPENDENCIES (Test TestAlgorithm)

OPTION (MAKE_PERFORMANCE_TESTS "Make (and run) performance tests" OFF)

IF (MAKE_PERFORMANCE_TESTS)
    ADD_EXECUTABLE (TestSortPerformance1 TestSortPerformance1.cpp)
    ADD_TEST (NAME TestSortPerformance1 COMMAND TestSortPerformance1)
    ADD_DEPENDENCIES (Test TestSortPerformance1)

    ADD_EXECUTABLE (TestSortPerformance2 TestSortPerformance2.cpp)
    ADD_TEST (NAME TestSortPerformance2 COMMAND TestSortPerformance2)
    ADD_DEPENDENCIES (Test TestSortPerformance2)
ENDIF ()

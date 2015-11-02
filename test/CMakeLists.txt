OPTION (MAKE_TESTS "Make tests when building" ON)
OPTION (EXECUTE_TESTS "Execute tests after building them" ON)
OPTION (EXECUTE_PERFORMANCE_TESTS "Execute some performance tests after building them" OFF)

IF (MAKE_TESTS)

    FIND_PACKAGE (Boost REQUIRED)

    INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES (../src)
    INCLUDE_DIRECTORIES (${GTEST_INCLUDE_DIRS})
    LINK_LIBRARIES (${GTEST_BOTH_LIBRARIES})

    IF (EXECUTE_TESTS)
        ADD_CUSTOM_TARGET (Test ALL COMMAND ctest -VV)
    ENDIF ()

    ADD_EXECUTABLE (TestIterator TestIterator.cpp)
    ADD_TEST (NAME TestIterator COMMAND TestIterator)

    ADD_EXECUTABLE (TestAlgorithm TestAlgorithm.cpp)
    ADD_TEST (NAME TestAlgorithm COMMAND TestAlgorithm)

    IF (EXECUTE_TESTS)
        ADD_DEPENDENCIES (Test TestIterator)
        ADD_DEPENDENCIES (Test TestAlgorithm)
    ENDIF ()

    ADD_EXECUTABLE (TestSortPerformance1 TestSortPerformance1.cpp)
    ADD_EXECUTABLE (TestSortPerformance2 TestSortPerformance2.cpp)
    IF (EXECUTE_PERFORMANCE_TESTS)
        ADD_TEST (NAME TestSortPerformance1 COMMAND TestSortPerformance1)
        ADD_TEST (NAME TestSortPerformance2 COMMAND TestSortPerformance2)
        ADD_DEPENDENCIES (Test TestSortPerformance1)
        ADD_DEPENDENCIES (Test TestSortPerformance2)
    ENDIF ()

ENDIF () # MAKE_TESTS

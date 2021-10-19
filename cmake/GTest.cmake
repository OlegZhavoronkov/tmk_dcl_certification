enable_testing()
include(GoogleTest)
set(OLD_ROOT_DIR "" CACHE INTERNAL "For CMake debug output")
set(OLD_TEST_LOCAL_DATA_DIR "" CACHE INTERNAL "For CMake debug output")

# Modifies target to include all compilation and linkage options
# for googletest-based test.
# Optional arguments:
# ROOT_DIR - directory containing test data folder. Default value is project root.
# TEST_LOCAL_DATA_DIR - directory for temporary test data. Default value is build directory.
function(link_gtest TARGET)
    if(${ARGC} GREATER 1)
        set(ROOT_DIR ${ARGV1})
    else()
        set(ROOT_DIR ${PROJECT_SOURCE_DIR})
        if(NOT OLD_ROOT_DIR OR NOT "${OLD_ROOT_DIR}" STREQUAL "${ROOT_DIR}")
            message("ROOT_DIR=${ROOT_DIR}")
            set(OLD_ROOT_DIR ${ROOT_DIR} CACHE INTERNAL "For CMake debug output")
        endif()
    endif()

    if(${ARGC} GREATER 2)
        set(TEST_LOCAL_DATA_DIR ${ARGV2})
    else()
        set(TEST_LOCAL_DATA_DIR "${PROJECT_BINARY_DIR}/test_data")
        if(NOT OLD_TEST_LOCAL_DATA_DIR OR NOT "${OLD_TEST_LOCAL_DATA_DIR}" STREQUAL "${TEST_LOCAL_DATA_DIR}")
            message("TEST_LOCAL_DATA_DIR=${TEST_LOCAL_DATA_DIR}")
            set(OLD_TEST_LOCAL_DATA_DIR ${TEST_LOCAL_DATA_DIR} CACHE INTERNAL "For CMake debug output")
        endif()
    endif()

    target_compile_definitions(${TARGET} PRIVATE TEST_LOCAL_DATA_DIR="${TEST_LOCAL_DATA_DIR}"  ROOT_DIR="${ROOT_DIR}")
endfunction()

# Calls link_gtest with given arguments and registers tests in CTest.
macro(add_gtest TARGET)
    link_gtest(${ARGV})
    gtest_discover_tests(${TARGET})
endmacro()

cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "savemiddle")

project(${PROJECT_NAME}
    DESCRIPTION "Save data middle block"
    HOMEPAGE_URL "https://github.com/tmk-org/TMK-DCL2/dclcomponents/savemiddle/"
    VERSION 0.0.1
    LANGUAGES C CXX
)

###############################################################################
# Options
###############################################################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -g")

###############################################################################
# Include CMake dependencies
###############################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

###############################################################################
# Add sources
###############################################################################

add_library(${PROJECT_NAME} SHARED
    #headers
    include/savemiddle.h
    #sources
    src/savemiddle.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    rt # time
    dclcapture-tcp
    dclprocessor-client
    glog::glog
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
)
target_include_directories(${PROJECT_NAME} PUBLIC
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
     $<INSTALL_INTERFACE:include>
)

###############################################################################
# Install
###############################################################################

###############################################################################
# Executable
###############################################################################

add_executable(save_middle src/SaveMiddleBlock.cpp)
target_link_libraries(save_middle savemiddle dclprocessor-client)

###############################################################################
# Tests
###############################################################################

if (NOT WITHOUT_TESTING)
    if (NOT SAVEMIDDLE_TEST_DATA)
        if (TEST_DATA_ROOT)
            set(SAVEMIDDLE_TEST_DATA "${TEST_DATA_ROOT}/savemiddle/")
        else()
            set(SAVEMIDDLE_TEST_DATA "../data/savemiddle/")
        endif()
    endif()

    find_package(GTest 1.8.1 REQUIRED) # using GTEST_SKIP requires at least this version
    set(PROJECT_TESTS ${PROJECT_NAME}-tests)

    add_executable(${PROJECT_TESTS}
            tests/SaveMiddleBlockTests.cpp
            )

    target_link_libraries(${PROJECT_TESTS}
            ${GTEST_BOTH_LIBRARIES} -pthread rt
            ${PROJECT_NAME}
            dclprocessor-storage storage Boost::boost ${OpenCV_LIBS}
            )
    set_property(TARGET ${PROJECT_TESTS} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
    set_property(TARGET ${PROJECT_TESTS} PROPERTY COMPILE_DEFINITIONS "SAVEMIDDLE_TEST_DATA=\"${SAVEMIDDLE_TEST_DATA}\"")
    add_test(NAME ${PROJECT_TESTS} COMMAND $<TARGET_FILE:${PROJECT_TESTS}>)
endif ()

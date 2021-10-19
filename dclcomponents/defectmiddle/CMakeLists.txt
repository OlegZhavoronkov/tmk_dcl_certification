cmake_minimum_required(VERSION 3.16)

set(LIBRARY_NAME "defectmiddle")

project(${LIBRARY_NAME}
    DESCRIPTION "Defect flaw detector middle block"
    HOMEPAGE_URL "https://github.com/tmk-org/TMK-DCL2/dclcomponents/defectmiddle"
    VERSION 0.0.1
    LANGUAGES C CXX)

find_package(OpenCV)
find_package(Torch)

###############################################################################
# Options
###############################################################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -g")

###############################################################################
# Add sources
###############################################################################

add_library(${LIBRARY_NAME} SHARED
    #headers
    include/defectdetection.h
    #include/DefectMiddle.h
    #sources
    src/defectdetection.cpp
    #src/DefectMiddle.cpp
)

target_link_libraries(${LIBRARY_NAME} PUBLIC -lrt "${TORCH_LIBRARIES}" ${OpenCV_LIBS} opencv_core)
target_include_directories(${LIBRARY_NAME} PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

###############################################################################
# Install
###############################################################################

add_executable(example_defect_middle src/DefectMiddle.cpp src/defectmiddleblock.cpp)
target_link_libraries(example_defect_middle defectmiddle dclprocessor-client)

###############################################################################
# Tests
###############################################################################

if (NOT WITHOUT_TESTING)
    set(PROJECT_TESTS ${LIBRARY_NAME}-tests)
    add_executable(${PROJECT_TESTS}
        src/DefectMiddle.cpp
        tests/DefectDetectionMiddleBlockTests.cpp
        )
    target_link_libraries(${PROJECT_TESTS}
        ${GTEST_BOTH_LIBRARIES} -pthread rt
        ${LIBRARY_NAME}
        dclprocessor-storage storage
        dclcapture-tcp
        ${OpenCV_LIBS}
        )
    set_property(TARGET ${PROJECT_TESTS} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
    add_test(NAME ${PROJECT_TESTS} COMMAND $<TARGET_FILE:${PROJECT_TESTS}>)
endif()

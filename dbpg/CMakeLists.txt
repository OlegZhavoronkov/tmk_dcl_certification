cmake_minimum_required(VERSION 3.16)
find_package(PQXX QUIET)
if(NOT PQXX_FOUND)
    return()
endif()
set(LIBRARY_NAME "dbpg")
project(${LIBRARY_NAME} VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PQXX REQUIRED)

add_library(${LIBRARY_NAME}
    src/DescriptorsDB.cpp
    src/Filesystem.cpp

    include/dbpg/DescriptorsDB.h
)

target_include_directories(${LIBRARY_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(${LIBRARY_NAME} -lrt
    opencv_imgproc
    opencv_highgui
    glog::glog
    nlohmann_json::nlohmann_json
    ${PQXX_LIBRARIES}
    Boost::boost
    Boost::iostreams
    Boost::serialization
    storage
)

#### Tests ####
if (NOT WITHOUT_TESTING)
    set(LIBRARY_TESTS ${LIBRARY_NAME}-tests)
    add_executable(${LIBRARY_TESTS}
            tests/FsStorageTests.cpp
            )
    target_link_libraries(${LIBRARY_TESTS} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARIES} ${LIBRARY_NAME})
    target_compile_definitions(${LIBRARY_TESTS} PRIVATE TEST_LOCAL_DATA_DIR="${TEST_LOCAL_DATA_DIR}" ROOT_DIR="${ROOT_DIR}")
    set_property(TARGET ${LIBRARY_TESTS} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
    add_test(NAME ${LIBRARY_TESTS} COMMAND $<TARGET_FILE:${LIBRARY_TESTS}>)
endif ()
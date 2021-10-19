cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME "dclplconfig")

project(${PROJECT_NAME}
    DESCRIPTION "Json configuration file parser"
    HOMEPAGE_URL "https://github.com/tmk-org/DCL-TMK2/pipeline/config"
    LANGUAGES C CXX
)

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Glog REQUIRED)
# dclpipeline required -- check it and add include directories (or make cmake/FindDCLPipeline.cmake)

add_library(${PROJECT_NAME} SHARED
    #headers
    include/config/config.h
    #sources
    src/config.cpp
)

target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
    $<INSTALL_INTERFACE:include>
)

target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

#target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json) -- required
target_link_libraries(${PROJECT_NAME} glog::glog dclpipeline)

# Test
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(config_test tests/config_test.cpp)
target_link_libraries(config_test gtest pthread gtest gtest_main) #${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARIES}
target_link_libraries(config_test ${PROJECT_NAME})
add_test(NAME config-tests COMMAND $<TARGET_FILE:config_test>)

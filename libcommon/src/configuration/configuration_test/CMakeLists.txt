cmake_minimum_required(VERSION 3.16)
project(configuration_test 
        VERSION 0.0.1 
        DESCRIPTION "Common Infrastructure"
        LANGUAGES C CXX)
add_executable(configuration_test configuration_test.cpp)
include_directories(${PROJECT_INCLUDE_COMMON})
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME_INTERNAL})
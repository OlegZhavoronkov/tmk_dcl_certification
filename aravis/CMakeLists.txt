cmake_minimum_required(VERSION 3.18)
project(arv_capture C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(aravis REQUIRED)
find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)
#find_package(OpenCV REQUIRED)

add_library(${PROJECT_NAME} SHARED
        #sources
        src/CameraMetaData.cpp
        #headers
        include/arv_capture/CameraMetaData.h
        )
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/arv_capture>
        $<INSTALL_INTERFACE:include/arv_capture> ${aravis_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json ${aravis_LIBRARIES} boost_program_options boost_filesystem -pthread)

add_executable(cammeta tools/cammeta.cpp)
target_include_directories(cammeta PRIVATE include/arv_capture PUBLIC ${aravis_INCLUDE_DIRS})
target_link_libraries(cammeta PRIVATE ${PROJECT_NAME} nlohmann_json::nlohmann_json ${aravis_LIBRARIES} boost_program_options boost_filesystem -pthread)

add_executable(say2camera demos/main_say2camera.c)
target_include_directories(say2camera  PRIVATE include/arv_capture PUBLIC ${aravis_INCLUDE_DIRS})
target_link_libraries(say2camera  PRIVATE ${PROJECT_NAME})

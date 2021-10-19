cmake_minimum_required(VERSION 3.16)
project(aravis_capture_tools C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig)
pkg_check_modules (aravis aravis-0.8 REQUIRED)

add_library(arv_capture
        #sources
        src/arv_capture_thread.c
        src/CameraMetaData.cpp
        #headers
        include/arv_capture/arv_capture_thread.h
        include/arv_capture/CameraMetaData.h)

target_include_directories(arv_capture PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/arv_capture>
        $<INSTALL_INTERFACE:include/arv_capture> ${aravis_INCLUDE_DIRS})
target_link_libraries(arv_capture PRIVATE nlohmann_json::nlohmann_json ${aravis_LINK_LIBRARIES} boost_program_options boost_filesystem -pthread)

add_executable(demo_capture_thread demos/demo_capture_thread.cc)
add_dependencies(demo_capture_thread arv_capture)
target_link_libraries(demo_capture_thread  arv_capture ${aravis_LINK_LIBRARIES}
        opencv_core opencv_highgui opencv_imgproc -pthread X11)
target_include_directories(demo_capture_thread PUBLIC include/arv_capture ${aravis_INCLUDE_DIRS})

add_executable(cammeta tools/cammeta.cpp)
target_include_directories(cammeta PRIVATE include/arv_capture PUBLIC ${aravis_INCLUDE_DIRS})
target_link_libraries(cammeta PRIVATE arv_capture nlohmann_json::nlohmann_json ${aravis_LINK_LIBRARIES} boost_program_options boost_filesystem -pthread)

add_executable(say2camera demos/main_say2camera.c)
target_include_directories(say2camera  PRIVATE include/arv_capture PUBLIC ${aravis_INCLUDE_DIRS})
target_link_libraries(say2camera  PRIVATE arv_capture)

add_custom_target(aravis_capture_tools)
add_dependencies(aravis_capture_tools arv_capture demo_capture_thread cammeta say2camera)
cmake_minimum_required(VERSION 3.16)

set(LIBRARY_NAME "storage")
project(${LIBRARY_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glog)
find_package(Boost)
find_package(OpenCV)

add_library(${LIBRARY_NAME} SHARED
    #sources
    src/ObjectDescriptor.cpp
    src/Frame.cpp
    src/Frameset.cpp
    src/Ipc.cpp
    src/IpcTypes.cpp
    src/Defect.cpp
    src/FrameData.cpp
    src/FrameDataSet.cpp
    src/ObjectData.cpp
    src/storage_interface.cpp
    #headers
    include/storage/ObjectDescriptor.h
    include/storage/Frame.h
    include/storage/Frameset.h
    include/storage/IObject.h
    include/storage/IpcTypes.h
    include/storage/Ipc.h
    include/storage/Time.h
    include/storage/Defect.h
    include/storage/FrameData.h
    include/storage/FrameDataSet.h
    include/storage/ObjectData.h
    include/storage/StorageTypes.h
    include/storage/storage_interface.h
)

target_include_directories(${LIBRARY_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(${LIBRARY_NAME} -lrt -lpthread opencv_imgproc opencv_highgui glog::glog Boost::boost misc)

#add_subdirectory(misc)

#### Tests ####
if (NOT WITHOUT_TESTING)
    find_package(GTest REQUIRED)
    enable_testing()
    set(LIBRARY_TESTS ${LIBRARY_NAME}-tests)
    add_executable(${LIBRARY_TESTS}
            tests/StorageTests.cpp
            )
    target_link_libraries(${LIBRARY_TESTS} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARIES} ${LIBRARY_NAME})
    target_compile_definitions(${LIBRARY_TESTS} PRIVATE TEST_LOCAL_DATA_DIR="${TEST_LOCAL_DATA_DIR}" ROOT_DIR="${ROOT_DIR}")
    set_property(TARGET ${LIBRARY_TESTS} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
    add_test(NAME ${LIBRARY_TESTS} COMMAND $<TARGET_FILE:${LIBRARY_TESTS}>)
endif ()
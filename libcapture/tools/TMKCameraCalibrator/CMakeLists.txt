cmake_minimum_required(VERSION 3.5)

project(TMKCameraCalibrator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})
find_package(nlohmann_json REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/supervised_calibration)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui

        # Algorithms
        CalibrationTools.cpp
        CalibrationTools.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TMKCameraCalibrator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(TMKCameraCalibrator
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(TMKCameraCalibrator PRIVATE
                          Qt${QT_VERSION_MAJOR}::Widgets
                          camera_models flat_pattern_calibration
                          nlohmann_json::nlohmann_json
                          opencv_highgui opencv_calib3d opencv_imgproc
)

set_target_properties(TMKCameraCalibrator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TMKCameraCalibrator)
endif()

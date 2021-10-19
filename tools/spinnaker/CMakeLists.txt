cmake_minimum_required (VERSION 3.16)
project(spinnaker_capture_tools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(X11 REQUIRED)
find_package(Spinnaker REQUIRED)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

#Spinnaker Acquisition demo appliaction. Taken here 'as is' to make shure that build is correct and SDK operates properly
add_executable(aqure_demo src/Acquisition.cpp)
target_link_libraries(aqure_demo PUBLIC ${SPINNAKER_LIBRARIES})
target_include_directories(aqure_demo PUBLIC include ${SPINNAKER_INCLUDE_DIRS})

#Spinnaker cameras enumerartor
add_executable(enum_cams src/main_enum_cams.cpp)
target_link_libraries(enum_cams PUBLIC ${SPINNAKER_LIBRARIES})
target_include_directories(enum_cams PUBLIC include ${SPINNAKER_INCLUDE_DIRS})

#Capturer
add_executable(aqure_images
        src/main_acquisition.cpp

        src/configparser.cpp
        src/configureCameras.cpp

        src/log.cpp
        src/namingSystems.cpp
        src/threadname.cpp
        src/visitors.cpp

        src/captureManager.cpp
        src/captureThread.cpp

        src/PreviewWidget.cpp

        include/configparser.h
        include/configureCameras.h

        include/global_sc.h
        include/log.h
        include/namingSystems.h
        include/threadname.h
        include/visitors.h

        include/captureManager.h
        include/captureThread.h

        include/PreviewWidget.h

        src/PreviewWidget.ui
        )
target_link_libraries(aqure_images PUBLIC ${SPINNAKER_LIBRARIES} pthread stdc++fs ${X11_LIBRARIES}
        ${REQUIRED_LIBS_QUALIFIED})
target_include_directories(aqure_images PUBLIC include src ${SPINNAKER_INCLUDE_DIRS})
set_property(TARGET aqure_images PROPERTY AUTOMOC ON)
set_property(TARGET aqure_images PROPERTY AUTORCC ON)
set_property(TARGET aqure_images PROPERTY AUTOUIC ON)

#Off-line raw frames decoder
add_executable(decode_flir_frame src/main_decode.cpp)
target_link_libraries(decode_flir_frame PUBLIC ${SPINNAKER_LIBRARIES} stdc++fs)
target_include_directories(decode_flir_frame PUBLIC ${SPINNAKER_INCLUDE_DIRS})

#Qt-based GUI test
add_executable(test_qtpreview src/main_test_qtpreview.cpp src/PreviewWidget.cpp include/PreviewWidget.h src/PreviewWidget.ui)
target_include_directories(test_qtpreview PUBLIC include src )
target_link_libraries(test_qtpreview PUBLIC pthread ${REQUIRED_LIBS_QUALIFIED})
set_property(TARGET test_qtpreview PROPERTY AUTOMOC ON)
set_property(TARGET test_qtpreview PROPERTY AUTORCC ON)
set_property(TARGET test_qtpreview PROPERTY AUTOUIC ON)

add_custom_target(spinnaker_capture_tools)
add_dependencies(spinnaker_capture_tools aqure_demo enum_cams aqure_images decode_flir_frame )


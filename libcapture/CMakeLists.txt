cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "dclcapture")

project(${PROJECT_NAME}
    DESCRIPTION "TCP and UDP source captuer for TMK-DCL"
    HOMEPAGE_URL "https://github.com/tmk-org/libcapture"
    VERSION 1.0.0
    LANGUAGES C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#file(MAKE_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/include)

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
# Options
###############################################################################

###############################################################################
# Find packages
###############################################################################

find_package(OpenCV REQUIRED)

###############################################################################
# Add sources
###############################################################################

add_library(${PROJECT_NAME}-tcp SHARED
    #headers
    include/tcpcapture/cvcamera.h
    include/tcpcapture/cvcollector.h
    include/tcpcapture/cvgenerator.h
    #sources
    src/cvcollector.cpp
    src/cvgenerator.cpp
)

target_link_libraries(${PROJECT_NAME}-tcp opencv_core opencv_highgui opencv_imgproc -pthread misc dclfilters storage)
target_include_directories(${PROJECT_NAME}-tcp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

add_library(${PROJECT_NAME}-udp SHARED
    #header
    include/udpcapture/iframe.h
    include/udpcapture/gvcp.h
    include/udpcapture/gvsp.h  #warning: no file gvsp.c, it's ok 
    include/udpcapture/device.h
    include/udpcapture/interface.h
    include/udpcapture/gige_camera_ops.h
    include/udpcapture/gige_camera.h
    #sources
    src/gvcp.c
    src/device.c
    src/interface.c
    src/gige_camera_ops.c
    src/gige_camera.c
)

target_link_libraries(${PROJECT_NAME}-udp misc -pthread)
target_include_directories(${PROJECT_NAME}-udp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

set(PUBLIC_HEADERS_UDP
    include/udpcapture/gvcp.h
    include/udpcapture/gvsp.h  #warning: no file gvsp.c, it's ok 
    include/udpcapture/device.h
    include/udpcapture/interface.h
    include/udpcapture/gige_camera.h
)

set_target_properties(${PROJECT_NAME}-udp PROPERTIES
    PUBLIC_HEADER "${CMAKE_BINARY_DIR}/${PUBLIC_HEADERS_UDP}"
)

set(PUBLIC_HEADERS_TCP
    include/tcpcapture/cvgenerator.h
    include/tcpcapture/cvcollector.h
    include/tcpcapture/cvcamera.h
)

set_target_properties(${PROJECT_NAME}-tcp PROPERTIES
    PUBLIC_HEADER "${CMAKE_BINARY_DIR}/${PUBLIC_HEADERS_TCP}"
)

#add_subdirectory(misc)
#add_subdirectory(libdclfilters)
#add_subdirectory(libstorage)

add_executable(tmk-camera tools/tcpcamera.cpp)
target_link_libraries(tmk-camera opencv_core opencv_highgui -pthread dclcapture-tcp)

add_executable(tmk-finder tools/udpfinder.c)
target_link_libraries(tmk-finder dclcapture-udp)

add_executable(tmk-test tools/udptest.c)
target_link_libraries(tmk-test dclcapture-udp)

###############################################################################
# Install
###############################################################################

foreach(HEADER ${PUBLIC_HEADERS_TCP})
    configure_file(${HEADER} ${CMAKE_BINARY_DIR}/${HEADER} COPYONLY)
    list(APPEND INSTALL_HEADERS ${CMAKE_BINARY_DIR}/${HEADER})
endforeach(HEADER)

foreach(HEADER ${PUBLIC_HEADERS_UDP})
    configure_file(${HEADER} ${CMAKE_BINARY_DIR}/${HEADER} COPYONLY)
    list(APPEND INSTALL_HEADERS ${CMAKE_BINARY_DIR}/${HEADER})
endforeach(HEADER)

if (UNIX)
    configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}-tcp
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
    install(TARGETS ${PROJECT_NAME}-udp
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
    install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
    install(FILES ${INSTALL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
endif ()


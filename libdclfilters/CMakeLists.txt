cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "dclfilters")

project(${PROJECT_NAME} 
    VERSION 0.0.1 
    DESCRIPTION "dclfilters library"
    LANGUAGES C CXX
)

set(LIBRARY_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -g")

# Packages
find_package(OpenCV REQUIRED)
find_package(Python3 COMPONENTS Interpreter)

add_library(${LIBRARY_NAME} SHARED
    #headers
    include/${LIBRARY_NAME}/cvfilter.h
    include/${LIBRARY_NAME}/cvparams.h
    include/${LIBRARY_NAME}/cvtools.h
    #sources
    src/cvfilter.cpp
    src/cvtools.cpp
    src/cvfilter_c.cpp
)

target_link_libraries(${LIBRARY_NAME} opencv_core opencv_highgui opencv_imgproc -pthread)
target_include_directories(${LIBRARY_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

set(PUBLIC_HEADERS_FILTERS
    include/${LIBRARY_NAME}/cvfilter.h
    include/${LIBRARY_NAME}/cvparams.h
    include/${LIBRARY_NAME}/cvtools.h
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    PUBLIC_HEADER "${CMAKE_BINARY_DIR}/${PUBLIC_HEADERS_FILTERS}"
)

###############################################################################
# Examples and tools
###############################################################################

add_executable(cvradius examples/cvradius.cpp)
target_link_libraries(cvradius opencv_core opencv_highgui opencv_imgproc ${LIBRARY_NAME})

add_executable(cvanalyzer tools/cvanalyzer.cpp)
target_link_libraries(cvanalyzer opencv_core opencv_highgui opencv_imgproc)

add_executable(cvshow tools/cvshow.cpp)
target_link_libraries(cvshow opencv_core opencv_highgui opencv_imgproc)

add_executable(radius examples/cvradius.c)
set_target_properties(radius PROPERTIES
                            LANGUAGE C
)
target_link_libraries(radius ${LIBRARY_NAME})

add_executable(raw2bmp tools/raw2bmp.c)
set_target_properties(raw2bmp PROPERTIES
                            LANGUAGE C
)
target_link_libraries(raw2bmp ${LIBRARY_NAME})

###############################################################################
# Install
###############################################################################

foreach(HEADER ${PUBLIC_HEADERS_FILTERS})
    configure_file(${HEADER} ${CMAKE_BINARY_DIR}/${HEADER} COPYONLY)
    list(APPEND INSTALL_HEADERS ${CMAKE_BINARY_DIR}/${HEADER})
endforeach(HEADER)

if (UNIX)
    configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
    install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
    install(FILES ${INSTALL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
endif ()

###############################################################################
# Tests
###############################################################################

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

set(PROJECT_TESTS ${PROJECT_NAME}-tests)

add_executable(${PROJECT_TESTS}
    tests/FilterTests.cpp
)

#add_executable(${PROJECT_TESTS}
#    tests/filter_tests.c
#)

target_link_libraries(${PROJECT_TESTS}
    ${GTEST_BOTH_LIBRARIES} pthread
    opencv_core opencv_highgui
    ${PROJECT_NAME}
)

add_test(NAME ${PROJECT_TESTS} COMMAND $<TARGET_FILE:${PROJECT_TESTS}>)

#add_test (NAME "${PROJECT_NAME}_test"
#  COMMAND "${Python3_EXECUTABLE} -m pytest ${PROJECT_SOURCE_DIR}/test.py"
#  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/"
#)





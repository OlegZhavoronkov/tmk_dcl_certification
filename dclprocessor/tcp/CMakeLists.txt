cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME "dclmessenger")

project(${PROJECT_NAME}
    DESCRIPTION "Network communication library"
    HOMEPAGE_URL "https://github.com/kat-spb/multiprocessor/messenger"
    LANGUAGES C)

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

###############################################################################
# Add sources
###############################################################################

#include(GenerateVersion)

set(DIR_TCP_NAME "tcp")

add_library(${PROJECT_NAME} SHARED
    #headers
    include/${DIR_TCP_NAME}/messenger.h
    include/${DIR_TCP_NAME}/tcp_process.h
    include/${DIR_TCP_NAME}/connection.h
    include/${DIR_TCP_NAME}/server.h
    include/${DIR_TCP_NAME}/client.h
    include/${DIR_TCP_NAME}/proxy.h
    include/${DIR_TCP_NAME}/command_server.h
    #sources
    src/server.c
    src/client.c
    src/proxy.c
    src/connection.c
    src/messenger.c
    src/tcp_process.c
    src/command_server.c
)

target_link_libraries (${PROJECT_NAME} misc pthread common dclpipeline dclplconfig)
target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set(PUBLIC_HEADERS_TCP
    include/${DIR_TCP_NAME}/connection.h
    include/${DIR_TCP_NAME}/server.h
    include/${DIR_TCP_NAME}/client.h
    include/${DIR_TCP_NAME}/proxy.h
    include/${DIR_TCP_NAME}/messenger.h
    include/${DIR_TCP_NAME}/tcp_process.h
    include/${DIR_TCP_NAME}/command_server.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${CMAKE_BINARY_DIR}/${PUBLIC_HEADERS_TCP}"
    LANGUAGE C
)

#add_subdirectory(misc)

#add_subdirectory(config)
add_subdirectory(examples)

###############################################################################
# Install
###############################################################################

foreach(HEADER ${PUBLIC_HEADERS_TCP})
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

#add_executable(${PROJECT_TESTS}
#    tests/msg_tests.c
#)

#target_link_libraries(${PROJECT_TESTS}
#    ${GTEST_BOTH_LIBRARIES} pthread
#    ${PROJECT_NAME}
#)

#add_test(${PROJECT_TESTS} ${PROJECT_TESTS})
#add_test(NAME ${PROJECT_TESTS} COMMAND $<TARGET_FILE:${PROJECT_TESTS}>

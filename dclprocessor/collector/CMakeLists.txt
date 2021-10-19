cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "dclcollector")

project(${PROJECT_NAME}
    DESCRIPTION "Engine for TMK_DCL system and client examples for this one"
    HOMEPAGE_URL "https://github.com/tmk-org/dclprocessor"
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

# libmisc.so required

###############################################################################
# Add sources
###############################################################################

SET(DIR_NAME "collector")

add_library(${PROJECT_NAME} SHARED
    #headers
    include/${DIR_NAME}/collector_description.h
    include/${DIR_NAME}/collector.h
    include/${DIR_NAME}/source_description.h
    include/${DIR_NAME}/source.h
    #sources
    src/source_description.c
    src/collector_description.c
    src/source.c
    src/collector.c
)

target_link_libraries(${PROJECT_NAME} dclcapture-tcp dclcapture-udp -pthread)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

set(PUBLIC_HEADERS_COLLECTOR
    include/${DIR_NAME}/collector.h
    include/${DIR_NAME}/collector_description.h
    include/${DIR_NAME}/source.h
    include/${DIR_NAME}/source_description.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${CMAKE_BINARY_DIR}/${PUBLIC_HEADERS_COLLECTOR}"
    LANGUAGE C
)

target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(tmk-collector examples/simple_collector.c)
target_link_libraries(tmk-collector ${PROJECT_NAME} misc)

#add_subdirectory(libcapture)
#add_subdirectory(config)
#add_subdirectory(examples)

###############################################################################
# Install
###############################################################################

foreach(HEADER ${PUBLIC_HEADERS_COLLECTOR})
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
if (NOT WITHOUT_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    set(PROJECT_TESTS ${PROJECT_NAME}-tests)

    add_executable(${PROJECT_TESTS}
            tests/DCLCollectorTests.cpp
            )

    target_link_libraries(${PROJECT_TESTS}
            ${GTEST_BOTH_LIBRARIES} -pthread rt
            ${PROJECT_NAME}
            )
    set_target_properties(${PROJECT_TESTS} PROPERTIES
                            LANGUAGE CXX
                            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
    add_test(NAME ${PROJECT_TESTS} COMMAND $<TARGET_FILE:${PROJECT_TESTS}>)
endif ()

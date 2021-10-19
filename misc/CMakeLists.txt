cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "misc")

project(${PROJECT_NAME}
    DESCRIPTION "Misc for TMK_DCL"
    HOMEPAGE_URL "https://github.com/tmk-org/dclprocessor"
    VERSION 1.0.0
    LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#file(MAKE_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
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

###############################################################################
# Add sources
###############################################################################

add_library(${PROJECT_NAME} SHARED
    #header
    include/misc/log.h
    include/misc/list.h
    include/misc/name_generator.h
    include/misc/ring_buffer.h
    #sources
    src/log.c
    src/ring_buffer.c
)

target_link_libraries(${PROJECT_NAME} -pthread)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

set(PUBLIC_HEADERS
    include/${PROJECT_NAME}/log.h
    include/${PROJECT_NAME}/list.h
    include/${PROJECT_NAME}/name_generator.h
    include/${PROJECT_NAME}/ring_buffer.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${CMAKE_BINARY_DIR}/${PUBLIC_HEADERS}"
)

target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

###############################################################################
# Install
###############################################################################

foreach(HEADER ${PUBLIC_HEADERS})
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
# Examples and tests
###############################################################################

add_executable(log_example examples/log_example.c)
target_link_libraries(log_example misc)

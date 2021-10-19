cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "dclpipeline")

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

add_library(dclshmem SHARED
    #header
    include/pipeline/shared_memory.h
    include/pipeline/shared_memory_map.h
    #sources
    src/shared_memory.c
)

target_link_libraries(dclshmem -lrt)
target_include_directories(dclshmem PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

add_library(dclproc SHARED
    #header
    include/pipeline/process_mngr.h
    include/pipeline/signal_reaction.h
    #sources
    src/process_mngr.c
    src/signal_reaction.c
)

target_link_libraries(dclproc -pthread -lrt)
target_include_directories(dclproc PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

add_library(${PROJECT_NAME} SHARED
    #headers
    include/pipeline/pipeline_description.h
    include/pipeline/pipeline.h
    include/pipeline/pipeline_server.h
    include/pipeline/pipeline_client.h
    include/pipeline/pipeline_module_description.h
    include/pipeline/pipeline_module.h
    #sources
    src/pipeline_description.c
    src/pipeline.c
    src/pipeline_server.c
    src/pipeline_client.c
    src/pipeline_module_description.c
    src/pipeline_module.c
)

target_link_libraries(${PROJECT_NAME} misc dclproc dclshmem -pthread rt)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

set(PUBLIC_HEADERS_PIPELINE
    include/pipeline/pipeline_module_description.h
    include/pipeline/pipeline_description.h
    include/pipeline/pipeline.h
    include/pipeline/config/config.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${CMAKE_BINARY_DIR}/${PUBLIC_HEADERS_PIPELINE}"
    LANGUAGE C
)

target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

#add_subdirectory(misc)

add_subdirectory(config)
add_subdirectory(examples)

###############################################################################
# Install
###############################################################################

foreach(HEADER ${PUBLIC_HEADERS_PIPELINE})
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
            tests/DCLPipelineTests.cpp
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

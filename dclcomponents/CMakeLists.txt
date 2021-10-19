cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "components")

project(${PROJECT_NAME}
    DESCRIPTION "Components of TMK_DCL system (services and modules)"
    HOMEPAGE_URL "https://github.com/tmk-org/TMK-DCL2"
    VERSION 1.0.0
    LANGUAGES C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -g")

set_component_default_state(pipelinedemo ON)
add_component(pipelinedemo)

add_executable(tmk-first first.cpp)
target_link_libraries(tmk-first dclprocessor misc storage)

add_executable(tmk-middle middle.cpp)
target_link_libraries(tmk-middle  dclprocessor misc storage)

add_executable(tmk-pg pgmiddle.cpp)
target_link_libraries(tmk-pg dclprocessor misc  dclprocessor storage glog::glog pqxx)

add_executable(tmk-last last.cpp)
target_link_libraries(tmk-last dclprocessor misc storage)

add_executable(tmk-server server.c)
target_link_libraries(tmk-server dclprocessor misc)

add_executable(tmk-manager manager.cpp)
target_link_libraries(tmk-manager dclmessenger)

###############################################################################
# Components
###############################################################################
#set_component_default_state(pgmiddle ON)
#add_component(pgmiddle)

set_component_default_state(savemiddle OFF)
add_component(savemiddle)

set_component_default_state(defectmiddle OFF)
add_component(defectmiddle)

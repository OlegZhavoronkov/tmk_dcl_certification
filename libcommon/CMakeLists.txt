cmake_minimum_required(VERSION 3.16)
project(common 
        VERSION 0.0.1 
        DESCRIPTION "Common Infrastructure"
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_NAME ${PROJECT_NAME})
set(PROJECT_NAME_INTERNAL ${PROJECT_NAME})
set(BUILD_CONFIGURATION_TEST OFF)
#set(Boost_USE_STATIC_LIBS ON) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME ON) 

add_library(${PROJECT_NAME} SHARED "" )
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json PUBLIC -lrt -lpthread)
target_include_directories(${PROJECT_NAME}
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                            SYSTEM ${Boost_INCLUDE_DIRS})
#set(WITHOUT_TESTING FALSE)

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/configurator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/meta_info.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/config_factory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/ServerSettings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/config_exceptions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/PipelineConfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/FakeCameraConfig.cpp
#C style interface
        ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/configuration_c.cpp
#headers
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/configurator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/configuration_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/meta_info.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/IJsonSaveable.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/config_exceptions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/PipelineConfig.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/server_settings_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/ServerSettings.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/FakeCameraConfig.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/config_factory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libcommon/time_utils.h)

if(${BUILD_CONFIGURATION_TEST} )
    add_subdirectory(src/configuration/configuration_test)
endif()

if (NOT WITHOUT_TESTING)
    add_subdirectory(tests/configuration)
endif ()

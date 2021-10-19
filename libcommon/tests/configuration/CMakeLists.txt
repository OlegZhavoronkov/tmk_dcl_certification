cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(LIBRARY_TESTS ${LIBRARY_NAME}-configuration-tests)

include(GTest)
project(${LIBRARY_TESTS}
        LANGUAGES C CXX)
add_executable(${LIBRARY_TESTS}
        dedicated_configs_test.cpp
        load_save_test.cpp
        c_interface_test.cpp
        )
if(NOT TEMP_CONFIG_FILE)        
#set this variable to the actual path
        set(TEMP_CONFIG_FILE "temp_config.json")        
endif()
#message(STATUS "for ${LIBRARY_TESTS} \nGTEST_LIBRARY ${GTEST_LIBRARY}\n GTEST_MAIN_LIBRARIES ${GTEST_MAIN_LIBRARIES} \nLIBRARY_NAME ${LIBRARY_NAME} \n GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}")        
target_link_libraries(${LIBRARY_TESTS} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARIES} ${LIBRARY_NAME})
set_source_files_properties(load_save_test.cpp PROPERTIES COMPILE_DEFINITIONS "TEMP_CONFIG_FILE=\"${TEMP_CONFIG_FILE}\"")
set_source_files_properties(c_interface_test.cpp PROPERTIES COMPILE_DEFINITIONS "TEMP_CONFIG_FILE=\"${TEMP_CONFIG_FILE}\"")
set_property(TARGET ${LIBRARY_TESTS} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
add_test(NAME ${LIBRARY_TESTS} COMMAND $<TARGET_FILE:${LIBRARY_TESTS}>)
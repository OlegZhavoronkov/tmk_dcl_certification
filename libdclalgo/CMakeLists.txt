cmake_minimum_required(VERSION 3.16)
project(dclalgo VERSION 0.0.2 DESCRIPTION "Polarization algorithms")
set(LIBRARY_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Ceres REQUIRED)

set(${LIBRARY_NAME}_SOURCE_FILES
        include/AzimuthEvaluation.h
        src/AzimuthEvaluation.cpp
        include/ZenithEvaluation.h
        src/ZenithEvaluation.cpp
        include/PolarDecomposition.h
        src/PolarDecomposition.cpp
        include/PolarExpand.h
        src/PolarExpand.cpp
        include/EdgeAware.h
        src/EdgeAware.cpp
        include/MapToMat.h
        include/interp1.h
        src/interp1.cpp
        include/CommonFunctions.h
        src/CommonFunctions.cpp
        include/LibraryParameters.h
        src/LibraryParameters.cpp
        include/ImageKeeper.h
        )
set(PUBLIC_HEADERS
        ${CMAKE_BINARY_DIR}/include/dclalgo_export.h
        ${CMAKE_BINARY_DIR}/include/AzimuthEvaluation.h
        ${CMAKE_BINARY_DIR}/include/ZenithEvaluation.h
        ${CMAKE_BINARY_DIR}/include/PolarDecomposition.h
        ${CMAKE_BINARY_DIR}/include/PolarExpand.h
        ${CMAKE_BINARY_DIR}/include/EdgeAware.h
        ${CMAKE_BINARY_DIR}/include/MapToMat.h
        ${CMAKE_BINARY_DIR}/include/interp1.h
        ${CMAKE_BINARY_DIR}/include/CommonFunctions.h
        ${CMAKE_BINARY_DIR}/include/LibraryParameters.h
        ${CMAKE_BINARY_DIR}/include/ImageKeeper.h
        )
add_library(${LIBRARY_NAME} SHARED ${${LIBRARY_NAME}_SOURCE_FILES} )

include(GenerateExportHeader)
generate_export_header(${LIBRARY_NAME}
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/dclalgo_export.h)

set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
target_compile_options(${LIBRARY_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(${LIBRARY_NAME} PRIVATE
        .
        include
        src
        ${CERES_INCLUDE_DIRS}
        INTERFACE
        $<INSTALL_INTERFACE:include/${LIBRARY_NAME}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PUBLIC
        $<INSTALL_INTERFACE:include/${LIBRARY_NAME}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        )

target_link_libraries(${LIBRARY_NAME} PUBLIC opencv_core opencv_highgui opencv_calib3d ${CERES_LIBRARIES} )

configure_file(include/AzimuthEvaluation.h ${CMAKE_BINARY_DIR}/include/AzimuthEvaluation.h COPYONLY)
configure_file(include/ZenithEvaluation.h ${CMAKE_BINARY_DIR}/include/ZenithEvaluation.h COPYONLY)
configure_file(include/PolarDecomposition.h ${CMAKE_BINARY_DIR}/include/PolarDecomposition.h COPYONLY)
configure_file(include/PolarExpand.h ${CMAKE_BINARY_DIR}/include/PolarExpand.h COPYONLY)
configure_file(include/EdgeAware.h ${CMAKE_BINARY_DIR}/include/EdgeAware.h COPYONLY)
configure_file(include/MapToMat.h ${CMAKE_BINARY_DIR}/include/MapToMat.h COPYONLY)
configure_file(include/interp1.h ${CMAKE_BINARY_DIR}/include/interp1.h COPYONLY)
configure_file(include/CommonFunctions.h ${CMAKE_BINARY_DIR}/include/CommonFunctions.h COPYONLY)
configure_file(include/LibraryParameters.h ${CMAKE_BINARY_DIR}/include/LibraryParameters.h COPYONLY)
configure_file(include/ImageKeeper.h ${CMAKE_BINARY_DIR}/include/ImageKeeper.h COPYONLY)

if (UNIX)
    configure_file(${LIBRARY_NAME}.pc.in ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}.pc @ONLY)
    include(GNUInstallDirs)
    install(TARGETS ${LIBRARY_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(FILES ${CMAKE_BINARY_DIR}/${LIBRARY_NAME}.pc
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
    install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME})
endif ()

# Tools
add_subdirectory(tools)

# Tests
if (NOT WITHOUT_TESTING)
    macro(add_algorithm_test TARGET)
        add_executable(${TARGET} tests/${TARGET}.cpp)
        target_link_libraries(${TARGET} PRIVATE ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARIES} ${LIBRARY_NAME}) #  Или что-то линковать как public?
        set_property(TARGET ${TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
        add_test(NAME ${TARGET} COMMAND $<TARGET_FILE:${TARGET}>)
    endmacro()

    add_algorithm_test(AzimuthEvaluationTests)

    add_algorithm_test(ZenithEvaluationTests)

    add_algorithm_test(PolarDecompositionTests)

    add_algorithm_test(PolarExpandTests)

    add_algorithm_test(MapToMatTests)

    add_algorithm_test(ImagesTests)

    add_algorithm_test(EdgeAwareTests)

    add_algorithm_test(ImageKeeperTests)
    if(NOT TEST_DATA_FOLDER)        
#set this variable to the actual path
        set(TEST_DATA_FOLDER "../data")        
    endif()
    set_source_files_properties(tests/ImagesTests.cpp PROPERTIES COMPILE_DEFINITIONS "TEST_DATA_FOLDER=\"${TEST_DATA_FOLDER}\"")
    set_source_files_properties(tests/EdgeAwareTests.cpp PROPERTIES COMPILE_DEFINITIONS "TEST_DATA_FOLDER=\"${TEST_DATA_FOLDER}\"")
endif ()
#
# On successfull identification the following variables will be defined
#
# ARAVIS_FOUND       - system has aravis
# ARAVIS_INCLUDE_DIR - include directories
# ARAVIS_LIBRARIES   - linker flags
# ARAVIS_DEFINITIONS - Compiler flags required by aravis
#

include(FindPkgConfig)


function(dump_cmake_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)

            #case sensitive match
            # string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
            #
            #case insenstitive match
            string( TOLOWER "${ARGV0}" ARGV0_lower )
            string( TOLOWER "${_variableName}" _variableName_lower )
            string(REGEX MATCH ${ARGV0_lower} MATCHED ${_variableName_lower})

            if (NOT MATCHED)
                continue()
            endif()
        endif()
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

function(CheckArvModuleVersion ver)
    pkg_check_modules (ARAVISver "aravis-${ver}")
    if(ARAVISver_FOUND)
        set(ARAVIS_VERSION "aravis-${ver}" PARENT_SCOPE)
        else()
        set(ARAVIS_VERSION "NOT_FOUND" PARENT_SCOPE)
    endif()
endfunction()

CheckArvModuleVersion("0.4")
CheckArvModuleVersion("0.6")
CheckArvModuleVersion("0.8")

if("${ARAVIS_VERSION}" STREQUAL "NOT_FOUND")
    find_path(aravis_INCLUDE_DIR
            NAMES
            arv.h
            PATHS
            /usr/local/include
            /usr/local/include/aravis-0.4
            /usr/local/include/aravis-0.6
            /usr/local/include/aravis-0.8
            /usr/include
            /usr/include/aravis-0.4
            /usr/include/aravis-0.6
            /usr/include/aravis-0.8
            )

    # Finally the library itself
    find_library(aravis_LIBRARIES
            NAMES
            libaravis-0.4
            libaravis-0.6
            libaravis-0.8
            aravis
            aravis-0.4
            aravis-0.6
            aravis-0.8
            libaravis
            PATHS
            /usr/local/lib
            /usr/lib
            )
    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set ARAVIS_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(aravis  DEFAULT_MSG
            aravis_LIBRARIES aravis_INCLUDE_DIR)

else()
    pkg_check_modules (aravis "${ARAVIS_VERSION}")
endif()


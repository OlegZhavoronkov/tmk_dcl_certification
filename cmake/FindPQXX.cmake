# Find PostGreSQL C++ library and header file
# Sets
#   PQXX_FOUND                 to 0 or 1 depending on result
#   PQXX_INCLUDE_DIRECTORIES  to the directory containing mysql.h
#   PQXX_LIBRARIES            to the MySQL client library (and any dependents required)
# If PQXX_REQUIRED is defined, then a fatal error message will be generated if libpqxx is not found

if (NOT PQXX_FOUND)

    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(libpq libpq)
        pkg_check_modules(libpqxx libpqxx)
    endif()

    if (NOT libpq_FOUND)
        find_package(PostgreSQL)
        if (DEFINED PostgreSQL_INCLUDE_DIR AND DEFINED PostgreSQL_LIBRARY)
            set(libpq_LINK_LIBRARIES ${PostgreSQL_LIBRARY})
            set(libpq_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIR})
        endif ()
    endif ()

    if (NOT libpqxx_FOUND)
        file(TO_CMAKE_PATH "$ENV{PQXX_DIR}" _PQXX_DIR)

        find_library(libpqxx_LINK_LIBRARIES
                NAMES libpqxx pqxx
                PATHS
                ${_PQXX_DIR}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                /usr/local/pgsql/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                ${_PQXX_DIR}/lib
                ${_PQXX_DIR}
                ${CMAKE_INSTALL_PREFIX}/lib
                ${CMAKE_INSTALL_PREFIX}/bin
                /usr/local/pgsql/lib
                /usr/local/lib
                /usr/lib
                DOC "Location of libpqxx library"
                NO_DEFAULT_PATH
                )

        find_path(libpqxx_INCLUDE_DIRS
                NAMES pqxx/pqxx
                PATHS
                ${_PQXX_DIR}/include
                ${_PQXX_DIR}
                ${CMAKE_INSTALL_PREFIX}/include
                /usr/local/pgsql/include
                /usr/local/include
                /usr/include
                DOC "Path to pqxx/pqxx header file. Do not include the 'pqxx' directory in this value."
                NO_DEFAULT_PATH
                )
    endif ()
    find_package_handle_standard_args(PQXX
            REQUIRED_VARS libpqxx_LINK_LIBRARIES libpq_LINK_LIBRARIES libpq_INCLUDE_DIRS libpqxx_INCLUDE_DIRS)
    if (PQXX_FOUND)
        set(PQXX_FOUND 1 CACHE INTERNAL "PQXX found" FORCE)
        set(PQXX_INCLUDE_DIRS ${libpq_INCLUDE_DIRS} ${libpqxx_INCLUDE_DIRS} CACHE STRING "Include directories for PostGreSQL C++ library" FORCE)
        set(PQXX_LIBRARIES ${libpq_LINK_LIBRARIES} ${libpqxx_LINK_LIBRARIES} CACHE STRING "Link libraries for PostGreSQL C++ interface" FORCE)
    endif ()
endif ()

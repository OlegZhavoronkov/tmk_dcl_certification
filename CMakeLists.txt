cmake_minimum_required(VERSION 3.16)
project(tmk_dcl)

include(Config.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) ## Do we need it?

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE STRING "" FORCE)
endif()
file (TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX)

set(CMAKE_POSITION_INDEPENDENT_CODE True)

add_component(misc)
add_component(libdclfilters)
add_component(libcapture)
add_component(libdclalgo)
add_component(libstorage)

set_component_default_state(dbpg OFF)
add_component(dbpg)

add_component(dclprocessor)
add_component(dclcomponents)

#supplementary tools
set_component_default_state(aravis_capture_tools OFF) # supplementary tool
add_component(aravis_capture_tools tools/aravis EXCLUDE_FROM_ALL)

set_component_default_state(spinnaker_capture_tools OFF) # supplementary tool
add_component(spinnaker_capture_tools tools/spinnaker EXCLUDE_FROM_ALL)

add_component(libcommon)

cmake_minimum_required(VERSION 3.16)
project(dclalgotools VERSION 0.0.1 DESCRIPTION "Tools using DCL's algorithms")
set(CMAKE_CXX_STANDARD 20)

add_executable(Expander Expander.cpp)
target_link_libraries(Expander PRIVATE dclalgo)

add_executable(getImageMaps GetImageMaps.cpp)
target_link_libraries(getImageMaps PRIVATE dclalgo stdc++fs)

cmake_minimum_required(VERSION 3.4.1)

project(secoc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/include ALL_INCLUDES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src ALL_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/utils ALL_UTILS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/test ALL_TEST)

add_executable(test ${ALL_TEST} ${ALL_UTILS})
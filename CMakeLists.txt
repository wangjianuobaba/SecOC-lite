cmake_minimum_required(VERSION 3.4.1)

project(secoc)
add_subdirectory(./googletest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
include_directories(./googletest/googletest/include)
include_directories(./googletest/googlemock/include)



set(CMAKE_CXX_STANDARD 11)
set(LIBRARIES gtest gtest_main pthread)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(UTILS ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(TEST_MASTER ${CMAKE_CURRENT_SOURCE_DIR}/test/test_master.cpp)
set(MASTER_INCLUDES
        ${INCLUDE}/masterFVM.h
        ${INCLUDE}/Compiler.h
        )
set(MASTER_SOURCES
        ${SOURCES}/masterFVM.c)
add_executable(MasterTest
        ${UTILS}
        ${TEST_MASTER}
        ${MASTER_INCLUDES}
        ${MASTER_SOURCES})

target_link_libraries(MasterTest ${LIBRARIES})
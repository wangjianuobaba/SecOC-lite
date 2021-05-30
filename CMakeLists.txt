cmake_minimum_required(VERSION 3.4.1)

project(secoc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(UTILS ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src)


## master test
set(TEST_MASTER ${CMAKE_CURRENT_SOURCE_DIR}/test/test_master.c)
set(MASTER_INCLUDES
        ${INCLUDE}/masterFVM.h
        ${INCLUDE}/MacGenerate.h
        ${INCLUDE}/sm4.h
        )
set(MASTER_SOURCES
        ${SOURCES}/masterFVM.c
        ${SOURCES}/MacGenerate.c
        ${SOURCES}/sm4.c
        )
add_executable(MasterTest
        ${UTILS}
        ${TEST_MASTER}
        ${MASTER_INCLUDES}
        ${MASTER_SOURCES})


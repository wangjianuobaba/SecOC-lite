cmake_minimum_required(VERSION 3.4.1)

project(secoc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(UTILS ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(TEST_MASTER ${CMAKE_CURRENT_SOURCE_DIR}/test/test_master.c)
set(TEST_SLAVE ${CMAKE_CURRENT_SOURCE_DIR}/test/test_slave.c)

## master test
# set(TEST_MASTER ${CMAKE_CURRENT_SOURCE_DIR}/test/test_master.c)
# set(MASTER_INCLUDES
#         ${INCLUDE}/masterFVM.h
#         ${INCLUDE}/Compiler.h
#         ${UTILS}/tools.h
#         )
# set(MASTER_SOURCES
#         ${SOURCES}/masterFVM.c
#         ${UTILS}/tools.c)
# add_executable(MasterTest
#         ${UTILS}
#         ${TEST_MASTER}
#         ${MASTER_INCLUDES}
#         ${MASTER_SOURCES})

## slave test
set(SLAVE_INCLUDES
        ${INCLUDE}/slaveFVM.h
        ${INCLUDE}/Compiler.h
        ${UTILS}/bitmap.h
        ${INCLUDE}/sm4.h
        ${INCLUDE}/MacGenerate.h
        )
set(SLAVE_SOURCES
        ${SOURCES}/slaveFVM.c
        ${UTILS}/bitmap.c
        ${SOURCES}/sm4.c
        ${SOURCES}/MacGenerate.c
        )
add_executable(SlaveTest
        ${TEST_SLAVE}
        ${SLAVE_INCLUDES}
        ${SLAVE_SOURCES})

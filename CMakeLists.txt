cmake_minimum_required(VERSION 3.4.1)

project(secoc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/slave/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/slave/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)


set(UTILS ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(TEST_MASTER ${CMAKE_CURRENT_SOURCE_DIR}/test/test_master.c)
set(SLAVE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/slave/include/)
set(SLAVE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/slave/src/)

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
        ${SLAVE_INCLUDE}/slaveFVM.h
        ${SLAVE_INCLUDE}/Compiler.h
        ${UTILS}/bitmap.h
        ${SLAVE_INCLUDE}/sm4.h
        ${SLAVE_INCLUDE}/MacGenerate.h
        )
set(TEST_SLAVE ${CMAKE_CURRENT_SOURCE_DIR}/test/test_slave.c)
set(SLAVE_SOURCES
        ${SLAVE_SOURCE}/slaveFVM.c
        ${UTILS}/bitmap.c
        ${SLAVE_SOURCE}/sm4.c
        ${SLAVE_SOURCE}/MacGenerate.c
        )
add_executable(SlaveTest
        ${TEST_SLAVE}
        ${SLAVE_INCLUDES}
        ${SLAVE_SOURCES})

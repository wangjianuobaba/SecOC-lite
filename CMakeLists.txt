cmake_minimum_required(VERSION 3.4.1)

project(secoc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/master/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/slave/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/)

set(UTILS ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MASTER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/master/include)
set(SLAVE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/slave/include)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MASTER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/master/src)
set(SLAVE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/slave/src)

set(INCLUDES
        ${INCLUDE}/sm4.h
        ${INCLUDE}/MacGenerate.h
        )
set(SOURCES
        ${SOURCE}/sm4.c
        ${SOURCE}/MacGenerate.c
        )
set(UTILS_INCLUDES
        ${UTILS}/bitmap.h
        ${UTILS}/tools.h
        )
set(UTILS_SOURCES
        ${UTILS}/bitmap.c
        ${UTILS}/tools.c
        )
## master test
#set(TEST_MASTER ${CMAKE_CURRENT_SOURCE_DIR}/test/test_master.c)
#set(MASTER_INCLUDES
#        ${MASTER_INCLUDE}/masterFVM.h
#        )
#set(MASTER_SOURCES
#        ${MASTER_SOURCE}/masterFVM.c
#        )
#add_executable(MasterTest
#        ${TEST_MASTER}
#        ${UTILS_INCLUDES}
#        ${UTILS_SOURCES}
#        ${INCLUDES}
#        ${SOURCES}
#        ${MASTER_INCLUDES}
#        ${MASTER_SOURCES})

## slave test
#set(TEST_SLAVE ${CMAKE_CURRENT_SOURCE_DIR}/test/test_slave.c)
#set(SLAVE_INCLUDES
#        ${SLAVE_INCLUDE}/slaveFVM.h
#        )
#set(SLAVE_SOURCES
#        ${SLAVE_SOURCE}/slaveFVM.c
#        )
#add_executable(SlaveTest
#        ${TEST_SLAVE}
#        ${UTILS_INCLUDES}
#        ${UTILS_SOURCES}
#        ${INCLUDES}
#        ${SOURCES}
#        ${SLAVE_INCLUDES}
#        ${SLAVE_SOURCES})

## test
set(TEST ${CMAKE_CURRENT_SOURCE_DIR}/test/test.c)
set(TEST_INCLUDES
        ${SLAVE_INCLUDE}/slaveFVM.h
        ${MASTER_INCLUDE}/masterFVM.h
        )
set(TEST_SOURCES
        ${SLAVE_SOURCE}/slaveFVM.c
        ${MASTER_SOURCE}/masterFVM.c
        )
add_executable(SlaveTest
        ${TEST}
        ${UTILS_INCLUDES}
        ${UTILS_SOURCES}
        ${INCLUDES}
        ${SOURCES}
        ${TEST_INCLUDES}
        ${TEST_SOURCES})
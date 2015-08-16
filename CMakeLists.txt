project(mBrane)

cmake_minimum_required(VERSION 2.8)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=gnu++0x -fPIC -g")# -Wfatal-errors")
endif()
include_directories(${PROJECT_SOURCE_DIR}/ ${PROJECT_SOURCE_DIR}/CoreLibrary ${PROJECT_SOURCE_DIR}/Core)

set (mBrane_srcs
    mBrane/messaging.cpp
    mBrane/networking.cpp
    mBrane/main.cpp
    mBrane/network_id.cpp
    mBrane/node.cpp
    mBrane/executing.cpp
    mBrane/space.cpp
    mBrane/module_descriptor.cpp
    )

set (mBrane_hdrs
    mBrane/messaging.h
    mBrane/space.h
    mBrane/mBrane.h
    mBrane/node.h
    mBrane/network_id.h
    mBrane/networking.h
    mBrane/module_descriptor.h
    mBrane/executing.h
    mBrane/projection.h
    )

add_executable(mBrane ${mBrane_srcs})
target_link_libraries(mBrane Core pthread CoreLibrary dl)
add_subdirectory(UDP)
add_subdirectory(TCP)
add_subdirectory(Core)
add_subdirectory(pingpong)
add_subdirectory(Perf)
add_subdirectory(CoreLibrary)

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

# UDP module
set (UDP_srcs
    UDP/udp_interface.cpp
    UDP/udp_channel.cpp
    UDP/udp.cpp
    )
set (UDP_hdrs
    UDP/udp_interface.h
    UDP/udp_channel.h
    UDP/udp.h
    )
add_library(udp SHARED ${UDP_srcs})

# TCP module
set (TCP_srcs
    TCP/tcp.cpp
    TCP/tcp_channel.cpp
    TCP/tcp_interface.cpp
    )
set (TCP_hdrs
    TCP/tcp.h
    TCP/tcp_channel.h
    TCP/tcp_interface.h
    )
add_library(tcp SHARED ${TCP_srcs})

# pingpong module
set (Pingpong_srcs
    pingpong/modules.cpp
    pingpong/pingpong.cpp
    )
set (Pingpong_hdrs
    pingpong/pingpong.h
    pingpong/pingpong_class.h
    pingpong/pingpong_class_def.h
    pingpong/pingpong_modules.h
    )
add_library(pingpong ${Pingpong_srcs})

# Perf module
set (Perf_srcs
    Perf/Perf.cpp
    Perf/modules.cpp
    )
set (Perf_hdrs
    Perf/Perf.h
    Perf/Perf_class.h
    Perf/Perf_class_def.h
    Perf/Perf_modules.h
    Perf/RR.h
    )
add_library(perf ${Perf_srcs})


add_subdirectory(Core)
add_subdirectory(CoreLibrary)

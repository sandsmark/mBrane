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
    Core/class_register.cpp
    Core/message.cpp
    Core/module_register.cpp
    Core/module_node.cpp
    Core/payload.cpp
    Core/network_interface.cpp
    Core/mdaemon_node.cpp
    Core/control_messages.cpp
    Core/memory.cpp
    Core/module.cpp
    TCP/tcp_channel.cpp
    TCP/tcp_interface.cpp
    TCP/tcp.cpp
    pingpong/pingpong.cpp
    pingpong/modules.cpp
    UDP/udp_interface.cpp
    UDP/udp_channel.cpp
    UDP/udp.cpp
    CoreLibrary/base.cpp
    CoreLibrary/utils.cpp
    CoreLibrary/xml_parser.cpp
    Perf/modules.cpp
    Perf/Perf.cpp
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
    Core/object.h
    Core/CoreTest.h
    Core/module_register.h
    Core/memory.h
    Core/list.h
    Core/application.h
    Core/mdaemon_node.h
    Core/mBrane.h
    Core/network_interface.h
    Core/module.h
    Core/payload_utils.h
    Core/dynamic_class_loader.h
    Core/library.h
    Core/control_messages.h
    Core/module_node.h
    Core/config.h
    Core/payload.h
    Core/array.h
    Core/mBrane_message_classes.h
    Core/message.h
    Core/classes.h
    Core/class_register.h
    TCP/tcp_channel.h
    TCP/tcp.h
    TCP/tcp_interface.h
    pingpong/pingpong.h
    pingpong/pingpong_class_def.h
    pingpong/pingpong_modules.h
    pingpong/pingpong_class.h
    UDP/udp_interface.h
    UDP/udp_channel.h
    UDP/udp.h
    CoreLibrary/utils.h
    CoreLibrary/pipe.h
    CoreLibrary/xml_parser.h
    CoreLibrary/types.h
    CoreLibrary/base.h
    Perf/Perf_class.h
    Perf/Perf_class_def.h
    Perf/Perf.h
    Perf/RR.h
    Perf/Perf_modules.h
    )

add_library(mBrane ${mBrane_srcs})

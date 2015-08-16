set (UDP_srcs
    udp_interface.cpp
    udp_channel.cpp
    udp.cpp
    )

set (UDP_hdrs
    udp_interface.h
    udp_channel.h
    udp.h
    )

add_library(UDP ${UDP_srcs})

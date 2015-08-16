set (TCP_srcs
    tcp.cpp
    tcp_channel.cpp
    tcp_interface.cpp
    )

set (TCP_hdrs
    tcp.h
    tcp_channel.h
    tcp_interface.h
    )

add_library(TCP ${TCP_srcs})

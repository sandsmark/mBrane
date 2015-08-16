set (Pingpong_srcs
    modules.cpp
    pingpong.cpp
    )

set (Pingpong_hdrs
    pingpong.h
    pingpong_class.h
    pingpong_class_def.h
    pingpong_modules.h
    )

add_library(PingPong ${Pingpong_srcs})

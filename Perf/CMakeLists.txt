set (Perf_srcs
    Perf.cpp
    modules.cpp
    )

set (Perf_hdrs
    Perf.h
    Perf_class.h
    Perf_class_def.h
    Perf_modules.h
    RR.h
    )

add_library(Perf ${Perf_srcs})

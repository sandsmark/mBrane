set (CoreLibrary_srcs
    base.cpp
    utils.cpp
    xml_parser.cpp
    )

set (CoreLibrary_hdrs
    base.h
    pipe.h
    types.h
    utils.h
    xml_parser.h
    )

add_library(CoreLibrary ${CoreLibrary_srcs})

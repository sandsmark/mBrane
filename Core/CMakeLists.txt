set (Core_srcs
    class_register.cpp
    control_messages.cpp
    mdaemon_node.cpp
    memory.cpp
    message.cpp
    module.cpp
    module_node.cpp
    module_register.cpp
    network_interface.cpp
    payload.cpp
    )

set (Core_hdrs
    application.h
    array.h
    class_register.h
    classes.h
    config.h
    control_messages.h
    dynamic_class_loader.h
    library.h
    list.h
    mBrane.h
    mBrane_message_classes.h
    mdaemon_node.h
    memory.h
    message.h
    module.h
    module_node.h
    module_register.h
    network_interface.h
    object.h
    payload.h
    payload_utils.h
    )

add_library(Core ${Core_srcs})
target_link_libraries(Core CoreLibrary)

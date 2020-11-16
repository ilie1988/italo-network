add_library(italonet-android
    SHARED
    italonet_config.cpp
    italonet_daemon.cpp
    italonet_vpn.cpp)
add_log_tag(italonet-android)
target_link_libraries(italonet-android libitalonet)

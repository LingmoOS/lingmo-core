cmake_minimum_required(VERSION 3.14)
project(lingmo-xembedsniproxy)

add_definitions(-DQT_NO_CAST_TO_ASCII
                -DQT_NO_CAST_FROM_ASCII
                -DQT_NO_URL_CAST_FROM_STRING
                -DQT_NO_CAST_FROM_BYTEARRAY)

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(X11 REQUIRED)
find_package(XCB REQUIRED COMPONENTS XCB RANDR XFIXES DAMAGE COMPOSITE SHM UTIL IMAGE)
find_package(Qt6 REQUIRED COMPONENTS Core Gui DBus Widgets)
find_package(KF6WindowSystem REQUIRED)

set(XCB_LIBS
    XCB::XCB
    XCB::XFIXES
    XCB::DAMAGE
    XCB::COMPOSITE
    XCB::RANDR
    XCB::SHM
    XCB::UTIL
    XCB::IMAGE
)

set(XEMBED_SNI_PROXY_SOURCES
    main.cpp
    fdoselectionmanager.cpp
    snidbus.cpp
    sniproxy.cpp
    debug.cpp
    xtestsender.cpp
)

qt_add_dbus_adaptor(DBUS_SOURCES org.kde.StatusNotifierItem.xml sniproxy.h SNIProxy)
set(statusnotifierwatcher_xml org.kde.StatusNotifierWatcher.xml)
qt_add_dbus_interface(DBUS_SOURCES ${statusnotifierwatcher_xml} statusnotifierwatcher_interface)

set_source_files_properties(${DBUS_SOURCES} PROPERTIES SKIP_AUTOGEN ON)

add_executable(lingmo-xembedsniproxy ${XEMBED_SNI_PROXY_SOURCES} ${DBUS_SOURCES})

message(STATUS "X11 Libraries: ${X11_LIBRARIES}")

target_link_libraries(lingmo-xembedsniproxy
    Qt6::Core
    Qt6::Gui
    Qt6::DBus
    Qt6::Widgets
    KF6::WindowSystem
    ${XCB_LIBS}
    ${X11_LIBRARIES}  # 添加这个来确保链接X11库
)

install(TARGETS lingmo-xembedsniproxy DESTINATION ${CMAKE_INSTALL_BINDIR})

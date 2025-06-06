find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})


find_package(AppMenuGtkModule)
find_package(KF6WindowSystem)
find_package(KF6CoreAddons)
set_package_properties(AppMenuGtkModule PROPERTIES TYPE RUNTIME)

add_definitions(-DQT_NO_CAST_TO_ASCII
-DQT_NO_CAST_FROM_ASCII
-DQT_NO_CAST_FROM_BYTEARRAY)

find_package(XCB
    REQUIRED COMPONENTS
        XCB
)

set(GMENU_DBUSMENU_PROXY_SRCS
    extend/dbusmenutypes_p.cpp
#    extend/dbusmenushortcut_p.cpp

    main.cpp
    menuproxy.cpp
    window.cpp
    menu.cpp
    actions.cpp
    dbusmenuadaptor.cpp
    gdbusmenutypes_p.cpp
    icons.cpp
    utils.cpp
 )

# qt_add_dbus_adaptor(GMENU_DBUSMENU_PROXY_SRCS ./com.canonical.dbusmenu.xml window.h Window)

add_executable(lingmo-gmenuproxy ${GMENU_DBUSMENU_PROXY_SRCS})

set_package_properties(XCB PROPERTIES TYPE REQUIRED)

target_link_libraries(lingmo-gmenuproxy
    Qt6::Core
    Qt6::GuiPrivate
    Qt6::DBus
    Qt6::Widgets
    KF6::CoreAddons
    KF6::WindowSystem
    XCB::XCB
    ${AppMenuGtkModule}
)

configure_file(
    lingmo-gmenuproxy.service.in
    lingmo-gmenuproxy.service
    @ONLY
)

install(TARGETS lingmo-gmenuproxy DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lingmo-gmenuproxy.service DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/user/)

find_package(Qt5 COMPONENTS Core Widgets DBus X11Extras REQUIRED)
find_package(XCB MODULE REQUIRED COMPONENTS XCB KEYSYMS)
find_package(X11)

set(PROJECT_SOURCES
    main.cpp
    application.cpp
    QHotkey/qhotkey.cpp
    QHotkey/qhotkey_x11.cpp
)

add_executable(cute-chotkeys
    ${PROJECT_SOURCES}
)

target_link_libraries(cute-chotkeys
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::DBus
    Qt5::X11Extras
    ${XCB_LIBS}
    ${X11_LIBRARIES}
    XCB::KEYSYMS
)

install(TARGETS cute-chotkeys RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

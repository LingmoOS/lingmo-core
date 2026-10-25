project(lingmo-session)
set(TARGET lingmo-session)

pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET libsystemd)
find_package(Qt6 REQUIRED COMPONENTS Core)

set(SOURCES
    application.cpp
    autostartentries.cpp
    journalstream.cpp
    main.cpp
    processmanager.cpp
    restartpolicy.cpp
    compositorprocess.cpp
    xwaylandenvironment.cpp
    shellprocess.cpp
    shellconnection.cpp
    power.cpp
    userdirectories.cpp
)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET}
    lingmo-core-common
    Lingmo::SDKBase
    Qt6::Core
    PkgConfig::Systemd
)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(PROGRAMS lingmo-wayland-session DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES document-portal-stop.conf
    DESTINATION /usr/lib/systemd/user/xdg-document-portal.service.d
    RENAME lingmo-session.conf)

project(lingmo-session)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

set(TARGET lingmo-session)

set(SOURCES
        application.cpp
        main.cpp
        process.cpp
        processmanager.cpp
        networkproxymanager.cpp

        powermanager/power.cpp
        powermanager/powerproviders.cpp

        daemon-helper.cpp
        daemon-helper.h
)

qt5_add_dbus_adaptor(DBUS_SOURCES
                     com.lingmo.Session.xml
                     application.h Application
                     sessionadaptor SessionAdaptor)
set_source_files_properties(${DBUS_SOURCES} PROPERTIES SKIP_AUTOGEN ON)

find_package(KF5WindowSystem)
find_package(KF5CoreAddons)
find_package(Threads)

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMQtDeclareLoggingCategory)

ecm_qt_declare_logging_category(SOURCES  HEADER debug.h IDENTIFIER LINGMO_SESSION_D CATEGORY_NAME org.lingmo.session)

add_executable(${TARGET} ${SOURCES} ${DBUS_SOURCES})
target_link_libraries(${TARGET}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Quick
        Qt5::DBus
        Qt5::X11Extras
        KF5::WindowSystem
        KF5::CoreAddons
        ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES lingmo-xsession.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/xsessions/)
install(FILES lingmo-wayland-session.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/wayland-sessions/)

add_subdirectory(wayland_wrapper)

add_subdirectory(lib)

add_executable(lingmo_kwin_wayland_wrapper)
target_sources(lingmo_kwin_wayland_wrapper PRIVATE
    kwin_wrapper.cpp
    wl-socket.c
    signalhandler.cpp
    UpdateLaunchEnvironment.cpp
)

ecm_qt_declare_logging_category(lingmo_kwin_wayland_wrapper
    HEADER
        wrapper_logging.h
    IDENTIFIER
        KWIN_WRAPPER
    CATEGORY_NAME
    lingmo_kwin_wayland_wrapper
    DEFAULT_SEVERITY
        Warning
)

target_link_libraries(lingmo_kwin_wayland_wrapper Qt::Core Qt::DBus KWinXwaylandCommon)
set_property(TARGET lingmo_kwin_wayland_wrapper PROPERTY C_STANDARD 11)
install(TARGETS lingmo_kwin_wayland_wrapper ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

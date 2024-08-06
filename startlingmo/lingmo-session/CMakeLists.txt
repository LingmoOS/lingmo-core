set(lingmo_session_SRCS
    main.cpp
    startup.cpp
)

ecm_qt_declare_logging_category(lingmo_session_SRCS  HEADER debug.h IDENTIFIER LINGMO_SESSION CATEGORY_NAME org.lingmo.session)

qt_add_dbus_adaptor( lingmo_session_SRCS org.lingmo.Startup.xml startup.hpp Startup)

add_executable(lingmo_session ${lingmo_session_SRCS})

target_include_directories(lingmo_session PRIVATE ${CMAKE_SOURCE_DIR}/startlingmo ${CMAKE_BINARY_DIR}/startkde)
target_include_directories(lingmo_session PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lingmo_session
    startlingmo
)

install(TARGETS lingmo_session ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
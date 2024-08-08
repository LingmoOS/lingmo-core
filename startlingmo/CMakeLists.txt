set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(KF6_MIN_VERSION "6.1.0")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 COMPONENTS Core DBus REQUIRED)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(FindKWinPath)
include(ECMQtDeclareLoggingCategory)

find_kwin_wayland_bin_path()
find_kwin_bin_path()

configure_file(config-startlingmo.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-startlingmo.h)

find_package(KF5 REQUIRED COMPONENTS DBusAddons CoreAddons)

find_package(LibKWorkspace REQUIRED)

find_package(KF5Wayland REQUIRED)

SET(startlingmo_SRCS
    startlingmo.cpp 
    daemon-helper.cpp 
    UpdateLaunchEnvironment.cpp
    signalhandler.cpp
)

ecm_qt_declare_logging_category(startlingmo_SRCS  HEADER debug.h IDENTIFIER LINGMO_STARTUP CATEGORY_NAME org.lingmo.startup)

add_library(startlingmo OBJECT ${startlingmo_SRCS})
target_link_libraries(startlingmo 
    PUBLIC 
        Qt5::Core
        Qt5::DBus
        KF5::DBusAddons
        KF5::CoreAddons
        PW::KWorkspace
)

add_executable(startlingmo-wayland startlingmo-wayland.cpp)
target_link_libraries(startlingmo-wayland  
    PRIVATE 
        startlingmo
    PUBLIC
        Qt5::Core
        Qt5::DBus
)

add_subdirectory(lingmo-session) 
add_subdirectory(wayland_wrapper)

install(TARGETS startlingmo-wayland ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS lingmo-sourceenv.sh DESTINATION ${KDE_INSTALL_LIBEXECDIR})

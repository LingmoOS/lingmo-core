project(lingmo-services)
set(TARGET lingmo-services)

find_package(LingmoSDK REQUIRED)
find_package(LingmoClients REQUIRED COMPONENTS display)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ServicesSystemd REQUIRED IMPORTED_TARGET libsystemd)
pkg_check_modules(ServicesFontconfig REQUIRED IMPORTED_TARGET fontconfig)
pkg_check_modules(ServicesIcu REQUIRED IMPORTED_TARGET icu-uc)

add_subdirectory(clipboard)
add_subdirectory(idle)

file(READ appearance/fontconfig.conf FONTCONFIG_TEMPLATE)
configure_file(appearance/fontconfig_template.h.in ${CMAKE_CURRENT_BINARY_DIR}/fontconfig_template.h @ONLY)

add_executable(${TARGET}
    main.cpp
    application.cpp
    application.h
    dbuscall.h
    log.cpp
    log.h
    notification.cpp
    notification.h
    appearance/fontconfigwriter.cpp
    appearance/fontconfigwriter.h
    appearance/gtksettings.cpp
    appearance/gtksettings.h
    appearance/loginbackground.cpp
    appearance/loginbackground.h
    appearance/thememanager.cpp
    appearance/thememanager.h
    appearance/wallpapermanager.cpp
    appearance/wallpapermanager.h
    battery/battery.cpp
    battery/battery.h
    battery/batterywarning.cpp
    battery/batterywarning.h
    battery/powersupply.cpp
    battery/powersupply.h
    battery/upower.h
    brightness/brightnessmanager.cpp
    brightness/brightnessmanager.h
    brightness/ddcworker.cpp
    brightness/ddcworker.h
    crash/coredump.cpp
    crash/coredump.h
    crash/coredumpjournal.cpp
    crash/coredumpjournal.h
    crash/crashdialog.cpp
    crash/crashdialog.h
    crash/crashmanager.cpp
    crash/crashmanager.h
    locale/language.cpp
    locale/language.h
    nightlight/nightlight.cpp
    nightlight/nightlight.h
    permissions/permissionmanager.cpp
    permissions/permissionmanager.h
    permissions/permissionprompt.cpp
    permissions/permissionprompt.h
    power/displaypolicy.cpp
    power/displaypolicy.h
    power/powermanager.cpp
    power/powermanager.h
    proxy/proxyservice.cpp
    proxy/proxyservice.h
    schedule/scheduledfeature.cpp
    schedule/scheduledfeature.h
    schedule/scheduleutils.cpp
    schedule/scheduleutils.h
)
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/schedule
    ${CMAKE_CURRENT_BINARY_DIR}
)
set_target_properties(${TARGET} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

target_compile_definitions(${TARGET} PRIVATE
        LINGMO_GTK_CSS_DIR="${CMAKE_INSTALL_FULL_DATAROOTDIR}/${TARGET}/gtk"
)

# The sources name their Qt objects with QString; qt_interop.h converts at the
# toolkit's std::string boundary. Force-included so it is in scope everywhere.
target_compile_options(${TARGET} PRIVATE
    -include ${CMAKE_SOURCE_DIR}/qt_interop.h
)

target_link_libraries(${TARGET} PRIVATE
        Lingmo::SDK
        Lingmo::DisplayClient
        lingmo-core-common
        lingmo-clipboard-backend
        lingmo-idle-backend
        Qt6::Core
        PkgConfig::ServicesSystemd
        PkgConfig::ServicesFontconfig
        PkgConfig::ServicesIcu
)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES appearance/gtk/gtk-3.0.css appearance/gtk/gtk-4.0.css
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${TARGET}/gtk)
install(FILES brightness/60-lingmo-ddc.rules DESTINATION lib/udev/rules.d)
install(FILES brightness/lingmo-ddc.conf DESTINATION lib/modules-load.d)

project(lingmo-settings-daemon)
set(TARGET lingmo-settings-daemon)

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(X11 REQUIRED)
find_package(X11_XCB REQUIRED)
find_package(XCB REQUIRED)

pkg_check_modules(XORGLIBINPUT xorg-libinput IMPORTED_TARGET)
pkg_check_modules(XORGSERVER xorg-server IMPORTED_TARGET)
pkg_check_modules(SYNAPTICS xorg-synaptics IMPORTED_TARGET GLOBAL)

# 只查找当前目录的文件，不递归
file(GLOB SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# 分别添加各个子目录的文件
file(GLOB THEME_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/theme/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/theme/*.h"
)

file(GLOB BRIGHTNESS_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/brightness/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/brightness/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/brightness/*.c"
)

file(GLOB BATTERY_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/battery/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/battery/*.h"
)

file(GLOB LANGUAGE_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/language/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/language/*.h"
)

file(GLOB DOCK_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/dock/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/dock/*.h"
)

file(GLOB MOUSE_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/mouse/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/mouse/*.h"
)

file(GLOB TOUCHPAD_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/touchpad/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/touchpad/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/touchpad/x11/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/touchpad/x11/*.cpp"
)

# 合并所有源文件
set(SOURCES
    ${SRCS}
    ${THEME_SRCS}
    ${BRIGHTNESS_SRCS}
    ${BATTERY_SRCS}
    ${LANGUAGE_SRCS}
    ${DOCK_SRCS}
    ${MOUSE_SRCS}
    # ${TOUCHPAD_SRCS}
)

set(HEADERS "")
set(FORMS "")
set(RESOURCES "")

qt_add_dbus_adaptor(DBUS_SOURCES
                     brightness/com.lingmo.Brightness.xml
                     brightness/brightnessmanager.h BrightnessManager)
qt_add_dbus_adaptor(DBUS_SOURCES
                     theme/com.lingmo.Theme.xml
                     theme/thememanager.h ThemeManager)
qt_add_dbus_adaptor(DBUS_SOURCES
                     battery/com.lingmo.PrimaryBattery.xml
                     battery/battery.h Battery)
qt_add_dbus_adaptor(DBUS_SOURCES
                     language/com.lingmo.Language.xml
                     language/language.h Language)
qt_add_dbus_adaptor(DBUS_SOURCES
                     dock/com.lingmo.Dock.xml
                     dock/dock.h Dock)
qt_add_dbus_adaptor(DBUS_SOURCES
                     mouse/com.lingmo.Mouse.xml
                     mouse/mousemanager.h Mouse)
# qt_add_dbus_adaptor(DBUS_SOURCES
#                      touchpad/com.lingmo.Touchpad.xml
#                      touchpad/touchpadmanager.h TouchpadManager)

set_source_files_properties(${DBUS_SOURCES} PROPERTIES SKIP_AUTOGEN ON)

add_executable(${TARGET} ${SOURCES} ${DBUS_SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

file(GLOB TS_FILES translations/*.ts)
foreach(filepath ${TS_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND ts_files_replaced ${filename})
endforeach()
qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${ts_files_replaced})

target_link_libraries(${TARGET}
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::DBus
        Qt6::Xml
        ${X11_LIBRARIES}
        X11::X11
        X11::Xi
        X11::XCB
        ${XCB_LIBRARIES}
        PkgConfig::XORGLIBINPUT
        PkgConfig::XORGSERVER
        PkgConfig::SYNAPTICS
)

add_custom_target(translations DEPENDS ${QM_FILES} SOURCES ${ts_files_replaced})
add_dependencies(${TARGET} translations)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${TARGET}/translations)

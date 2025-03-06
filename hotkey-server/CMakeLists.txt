cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Libinput REQUIRED)
find_package(Qt6 COMPONENTS Core DBus Concurrent Gui REQUIRED)

#遍历所有Cpp文件
file(GLOB_RECURSE CPP_FILES *.cpp *.h)
foreach (filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sources_files ${filename})
endforeach (filepath)

qt6_add_dbus_adaptor(
    GENERATED_DBUS_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/org.lingmo.core.globalshortcut.xml 
    ${CMAKE_CURRENT_SOURCE_DIR}/global_shortcut_interface.h
    GlobalShortcutInterface
)
message(STATUS "Generated DBus sources: ${GENERATED_DBUS_SOURCES}")
list(APPEND sources_files ${GENERATED_DBUS_SOURCES})

add_executable(hotkey-server ${sources_files})

target_include_directories(hotkey-server 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Libinput_INCLUDE_DIRS}
)

target_link_libraries(hotkey-server 
    PRIVATE 
    Qt6::Core
    Qt6::DBus
    Qt6::Concurrent
    Qt6::Gui
    ${Libinput_LIBRARIES}
)
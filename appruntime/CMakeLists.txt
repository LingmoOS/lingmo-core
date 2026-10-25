// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

project(lingmo-appruntime)
set(TARGET lingmo-appruntime)

pkg_check_modules(Systemd REQUIRED IMPORTED_TARGET libsystemd)
find_package(Qt6 REQUIRED COMPONENTS Core)

set(SOURCES
    applicationscope.cpp
    applicationscope.h
    appruntime.cpp
    appruntime.h
    chromiumswitches.cpp
    chromiumswitches.h
    main.cpp
)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET}
    lingmo-core-common
    Lingmo::ToolkitBase
    Qt6::Core
    PkgConfig::Systemd
)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})

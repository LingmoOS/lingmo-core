project(lingmo-shutdown)

add_executable(lingmo-shutdown
    main.cpp
    actions.cpp
    qml.qrc
)

file(GLOB TS_FILES translations/*.ts)
foreach(filepath ${TS_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND ts_files_replaced ${filename})
endforeach()
qt6_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${ts_files_replaced})

target_link_libraries(lingmo-shutdown
        Qt6::Core
        Qt6::Widgets
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::DBus
)


add_custom_target(shutdown-translations DEPENDS ${QM_FILES} SOURCES ${ts_files_replaced})
add_dependencies(lingmo-shutdown shutdown-translations)

include(GNUInstallDirs)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/lingmo-shutdown/translations)

install(TARGETS lingmo-shutdown RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

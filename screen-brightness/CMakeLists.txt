project(cute-screen-brightness)
set(PROJECT_NAME cute-screen-brightness)

set(SRCS
    main.cpp
    brightnesshelper.h
    brightnesshelper.cpp
)

add_executable(${PROJECT_NAME}
    ${SRCS}
)

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

configure_file(
    com.cute.brightness.pkexec.policy.in
    com.cute.brightness.pkexec.policy
    @ONLY
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.cute.brightness.pkexec.policy DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/polkit-1/actions/)

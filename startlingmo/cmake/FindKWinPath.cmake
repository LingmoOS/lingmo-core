function(find_kwin_wayland_bin_path)
    # 使用 whereis 命令来查找 kwin 或 kwin_x11
    execute_process(
        COMMAND whereis kwin_wayland
        OUTPUT_VARIABLE kwin_wayland_output
        ERROR_QUIET
    )

    # 解析输出以获取二进制文件的路径
    string(REGEX MATCH "/[^\n\r ]*/kwin_wayland" kwin_wayland_bin_path "${kwin_wayland_output}")

    # 设置 KWIN_WAYLAND_BIN_PATH 变量
    set(KWIN_WAYLAND_BIN_PATH "${kwin_wayland_bin_path}" CACHE PATH "Path to the KWin Wayland binary" FORCE)
endfunction()

function(find_kwin_bin_path)
    # 使用 whereis 命令来查找 kwin 或 kwin_x11
    execute_process(
        COMMAND whereis kwin
        OUTPUT_VARIABLE kwin_output
        ERROR_QUIET
    )

    # 解析输出以获取二进制文件的路径
    string(REGEX MATCH "/[^\n\r ]*/kwin" kwin_bin_path "${kwin_output}")

    # 设置 KWIN_WAYLAND_BIN_PATH 变量
    set(KWIN_BIN "${kwin_bin_path}" CACHE PATH "Path to the KWin binary" FORCE)
endfunction()
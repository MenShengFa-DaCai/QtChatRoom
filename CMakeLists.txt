cmake_minimum_required(VERSION 3.28)
project(QtChatRoom)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/Qt6/6.9.1/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Sql
        REQUIRED)

qt_add_resources(RESOURCE_FILES
        icon/icon.qrc)

# 使用 WIN32 子系统避免控制台窗口
if(WIN32)
    add_executable(QtChatRoom WIN32 main.cpp
            login.cpp
            login.h
            login.ui
            chat.cpp
            chat.h
            chat.ui
            ${RESOURCE_FILES}
            message.cpp
            message.h
            message.ui
    )
else()
    add_executable(QtChatRoom main.cpp
            login.cpp
            login.h
            login.ui
            chat.cpp
            chat.h
            chat.ui
            ${RESOURCE_FILES}
            message.cpp
            message.h
            message.ui
    )
endif()

target_link_libraries(QtChatRoom
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Sql
)

# ========== Windows 平台 DLL 复制逻辑 ==========
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()

    # 创建临时目录用于存放运行时文件
    set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/runtime")
    file(MAKE_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}")
    file(MAKE_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms")
    file(MAKE_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/sqldrivers")
    file(MAKE_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/imageformats")

    # 复制平台插件
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        file(COPY "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms")
    endif ()

    # 复制Qt核心DLL
    set(QT_LIBS Core Gui Widgets Network Sql)
    foreach (QT_LIB IN LISTS QT_LIBS)
        file(COPY "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}")
    endforeach ()

    # 复制SQLite驱动
    file(COPY "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
            DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}/plugins/sqldrivers")

    # 复制图像格式插件
    file(COPY "${QT_INSTALL_PATH}/plugins/imageformats/qgif${DEBUG_SUFFIX}.dll"
            DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}/plugins/imageformats")
    file(COPY "${QT_INSTALL_PATH}/plugins/imageformats/qico${DEBUG_SUFFIX}.dll"
            DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}/plugins/imageformats")

    # 复制可执行文件
    add_custom_command(TARGET QtChatRoom POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:QtChatRoom>"
            "${RUNTIME_OUTPUT_DIRECTORY}")

    # 添加 MinGW 运行时库
    get_filename_component(MINGW_BIN_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)
    set(MINGW_RUNTIME_LIBS
            "${MINGW_BIN_PATH}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_PATH}/libstdc++-6.dll"
            "${MINGW_BIN_PATH}/libwinpthread-1.dll"
    )
    foreach(MINGW_LIB IN LISTS MINGW_RUNTIME_LIBS)
        file(COPY "${MINGW_LIB}" DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}")
    endforeach()

    # 添加 ICU 库
    file(GLOB ICU_DLLS "${QT_INSTALL_PATH}/bin/icu*.dll")
    foreach(ICU_DLL IN LISTS ICU_DLLS)
        file(COPY "${ICU_DLL}" DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}")
    endforeach()

    # 添加 OpenSSL 库
    set(OPENSSL_LIBS
            "libcrypto-3-x64.dll"
            "libssl-3-x64.dll"
    )
    foreach(SSL_LIB IN LISTS OPENSSL_LIBS)
        if (EXISTS "${QT_INSTALL_PATH}/bin/${SSL_LIB}")
            file(COPY "${QT_INSTALL_PATH}/bin/${SSL_LIB}"
                    DESTINATION "${RUNTIME_OUTPUT_DIRECTORY}")
        endif()
    endforeach()
endif ()

# ========== 安装规则 ==========
# 1. 安装可执行文件
install(FILES "${RUNTIME_OUTPUT_DIRECTORY}/QtChatRoom.exe"
        DESTINATION bin
        RENAME "QtChatRoom.exe"
)

# 2. 安装所有运行时 DLL（从 runtime 目录）
file(GLOB RUNTIME_DLLS "${RUNTIME_OUTPUT_DIRECTORY}/*.dll")
install(FILES ${RUNTIME_DLLS}
        DESTINATION bin
)

# 3. 安装 Qt 平台插件
install(DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms/"
        DESTINATION bin/plugins/platforms
        FILES_MATCHING PATTERN "*.dll"
)

# 4. 安装 Qt 数据库插件
install(DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/sqldrivers/"
        DESTINATION bin/plugins/sqldrivers
        FILES_MATCHING PATTERN "*.dll"
)

# 5. 安装图像格式插件
install(DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/plugins/imageformats/"
        DESTINATION bin/plugins/imageformats
        FILES_MATCHING PATTERN "*.dll"
)

# 6. 安装本地数据库
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LocalUsers.db"
        DESTINATION bin
        OPTIONAL
)

# ========== CPack 打包配置 ==========
set(CPACK_PACKAGE_NAME "QtChatRoom")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "聊天室客户端程序")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QtChatRoom")

set(CPACK_GENERATOR "NSIS")

set(ICON_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon/install.ico")
set(ICON_UNINSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon/uninstall.ico")

set(CPACK_NSIS_MUI_ICON "${ICON_INSTALL_PATH}")
set(CPACK_NSIS_MUI_UNIICON "${ICON_UNINSTALL_PATH}")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\QtChatRoom.exe")

# 自定义快捷方式
set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk' '$INSTDIR\\\\bin\\\\QtChatRoom.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk'")

# 开始菜单快捷方式
set(CPACK_NSIS_MENU_LINKS
        "bin/QtChatRoom.exe" "Qt Chat Room"
)

include(CPack)
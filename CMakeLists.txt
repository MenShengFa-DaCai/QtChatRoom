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
target_link_libraries(QtChatRoom
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Sql
)

# 安装目标
install(TARGETS QtChatRoom
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .
)

# 安装运行时依赖
if(WIN32)
    # 安装插件目录
    install(DIRECTORY $<TARGET_FILE_DIR:QtChatRoom>/plugins/
            DESTINATION bin/plugins
    )

    # 安装 DLL 文件
    install(FILES
            "$<TARGET_FILE:Qt6::Core>"
            "$<TARGET_FILE:Qt6::Gui>"
            "$<TARGET_FILE:Qt6::Widgets>"
            "$<TARGET_FILE:Qt6::Network>"
            "$<TARGET_FILE:Qt6::Sql>"
            DESTINATION bin
    )

    # 安装 Qt 插件
    get_target_property(QtCore_location Qt6::Core LOCATION)
    get_filename_component(QT_DLL_DIR "${QtCore_location}" DIRECTORY)

    install(FILES
            "${QT_DLL_DIR}/../plugins/platforms/qwindows.dll"
            DESTINATION bin/plugins/platforms
    )

    install(FILES
            "${QT_DLL_DIR}/../plugins/sqldrivers/qsqlite.dll"
            DESTINATION bin/plugins/sqldrivers
    )
endif()

# CPack 配置
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "QtChatRoom")
set(CPACK_PACKAGE_VENDOR "YourCompany")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt Chat Room Application")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QtChatRoom")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH OFF)

# 设置安装包图标
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon/install.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/icon/uninstall.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\QtChatRoom.exe")
set(CPACK_NSIS_DISPLAY_NAME "Qt Chat Room")
set(CPACK_NSIS_PACKAGE_NAME "Qt Chat Room")

# 创建桌面快捷方式
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
  CreateShortCut '$DESKTOP\\\\QtChatRoom.lnk' '$INSTDIR\\\\bin\\\\QtChatRoom.exe'
")
set(CPACK_NSIS_DELETE_ICONS_EXTRA "
  Delete '$DESKTOP\\\\QtChatRoom.lnk'
")

# 开始菜单快捷方式
set(CPACK_NSIS_MENU_LINKS
        "bin/QtChatRoom.exe" "Qt Chat Room"
)

include(CPack)
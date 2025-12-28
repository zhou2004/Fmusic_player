cmake_minimum_required(VERSION 3.30)

set(VERSION "1.0.0")

project(FMusic VERSION ${VERSION} LANGUAGES CXX)

set(APPLICATION_DIR_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_LIBDIR ".")

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Multimedia Network Sql LinguistTools Widgets PrintSupport)

qt_standard_project_setup()

# Option to enable SMTC support
option(ENABLE_SMTC "Enable SMTC support" ON)

if(ENABLE_SMTC)
    add_compile_definitions(ENABLE_SMTC=1)
endif()

# 查找所需的包
find_package(OPENSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(taglib CONFIG REQUIRED)
find_package(kissfft CONFIG REQUIRED)

file(GLOB PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/WinSMTCController.cpp")

# 嵌入可执行文件图标
# 1. 设置你的图标路径 (请修改这里为你的实际 .ico 路径)
set(APP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Assets/icon_256.ico")

# 2. 如果是 Windows，自动生成资源文件并添加到源列表中
if(WIN32)
    # 定义生成的 rc 文件路径（在 build 目录，不污染源码）
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_BINARY_DIR}/FMusic_icon.rc")

    # 写入内容：IDI_ICON1 ICON "path/to/icon.ico"
    # 注意：需要转义引号
    file(WRITE "${APP_ICON_RESOURCE_WINDOWS}" "IDI_ICON1 ICON \"${APP_ICON_PATH}\"")

    # 将生成的 rc 文件加入到你的源文件列表
    list(APPEND PROJECT_SOURCES "${APP_ICON_RESOURCE_WINDOWS}")
endif()

#设置信号与槽的控制类
file(GLOB_RECURSE HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/control/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/decrypt/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/db/*.h
)

qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Network
        Qt6::Multimedia
        Qt6::Sql
        Qt6::Widgets
        Qt6::PrintSupport
        fluentuiplugin
        taglib::tag
        kissfft::kissfft
        ZLIB::ZLIB
        OpenSSL::Crypto
        OpenSSL::SSL
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/control
        ${CMAKE_CURRENT_SOURCE_DIR}/include/decrypt
        ${CMAKE_CURRENT_SOURCE_DIR}/include/db
)

file(GLOB_RECURSE QML_FILES_IN_PAGE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Page/*.qml")

set(ALL_QML_FILES
        App.qml
        ${QML_FILES_IN_PAGE}
)

file(GLOB_RECURSE ASSETS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Assets/*")

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_qml_module(${PROJECT_NAME}
        URI "FMusic"
        VERSION 1.0
        QML_FILES
        ${ALL_QML_FILES}
        RESOURCES
        ${ASSETS_FILES}
)

if(ENABLE_SMTC)
    target_sources(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/cpp/WinSMTCController.cpp
    )

    find_package(cppwinrt QUIET)
    if(cppwinrt_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE cppwinrt::cppwinrt)
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE WindowsApp)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DWIN32_LEAN_AND_MEAN)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        RUNTIME_OUTPUT_DIRECTORY "${APPLICATION_DIR_PATH}"
)

# 国际化支持
file(GLOB TS_FILES "i18n/*.ts")

set(I18N_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/i18n")

foreach(ts_file ${TS_FILES})
    set_source_files_properties(${ts_file}
            PROPERTIES OUTPUT_LOCATION "${I18N_OUTPUT_PATH}"
    )
endforeach()

if (TS_FILES)

    add_custom_target(create_i18n_dir
            COMMAND ${CMAKE_COMMAND} -E make_directory "${I18N_OUTPUT_PATH}"
            COMMENT "Creating i18n directory structure..."
    )
    qt6_add_lrelease(${PROJECT_NAME}
            TS_FILES ${TS_FILES}
    )

endif()



# 3. 如果是 macOS (顺便处理一下 Mac 的图标)
if(APPLE)
    # Mac 需要 .icns 文件
    set(APP_ICON_MACOS "${CMAKE_CURRENT_SOURCE_DIR}/logo.icns")
    set_source_files_properties(${APP_ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND PROJECT_SOURCES "${APP_ICON_MACOS}")
endif()



qt_finalize_executable(${PROJECT_NAME})

include(GNUInstallDirs)

# 安装主程序
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION .
        BUNDLE  DESTINATION .
        LIBRARY DESTINATION .
)

# 安装翻译文件
install(DIRECTORY "${I18N_OUTPUT_PATH}/" DESTINATION "i18n")

# 生成并安装 Qt 部署脚本 (windeployqt 自动化)
qt_generate_deploy_qml_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT qt_deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
        DEPLOY_TOOL_OPTIONS "--no-translations --libdir ${CMAKE_INSTALL_LIBDIR}"
)
install(SCRIPT ${qt_deploy_script})

install(FILES "${CMAKE_SOURCE_DIR}/packages/AppxManifest.xml" DESTINATION .)
install(FILES "${CMAKE_SOURCE_DIR}/packages/priconfig.xml" DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/packages/Assets" DESTINATION .)
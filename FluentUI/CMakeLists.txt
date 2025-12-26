cmake_minimum_required(VERSION 3.30)

#编译参数设置
option(FLUENTUI_BUILD_STATIC_LIB "Build static library." OFF)

if (FLUENTUI_BUILD_STATIC_LIB)
    project(fluentui VERSION 1.0)
else ()
    project(fluentuiplugin VERSION 1.0)
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.cmake/)

#配置通用编译
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#导入Qt相关依赖包（显式使用 Qt6 配置，这会导入 Qt6 的 CMake helper 宏）
find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Widgets PrintSupport)

qt_standard_project_setup()

#设置版本号
add_definitions(-DFLUENTUI_VERSION=1,7,7,0)

if (FLUENTUI_BUILD_STATIC_LIB)
    add_definitions(-DFLUENTUI_BUILD_STATIC_LIB)
endif ()

#设置QML插件输出目录，可以通过外部设置
set(FLUENTUI_QML_PLUGIN_DIRECTORY "" CACHE PATH "Path to FluentUI plugin")
if (NOT FLUENTUI_QML_PLUGIN_DIRECTORY)
    set(FLUENTUI_QML_PLUGIN_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/FluentUI)
endif ()

qt_policy(SET QTP0004 OLD)

#国际化
find_program(QT_LUPDATE NAMES lupdate)
find_program(QT_LRELEASE NAMES lrelease)

set(I18N_DIR "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

if (NOT EXISTS ${I18N_DIR}/fluentui_en_US.qm)
    execute_process(COMMAND ${QT_LUPDATE} ${I18N_DIR} -ts fluentui_en_US.ts WORKING_DIRECTORY ${I18N_DIR})
    execute_process(COMMAND ${QT_LRELEASE} fluentui_en_US.ts WORKING_DIRECTORY ${I18N_DIR})
endif ()
if (NOT EXISTS ${I18N_DIR}/fluentui_zh_CN.qm)
    execute_process(COMMAND ${QT_LUPDATE} ${I18N_DIR} -ts fluentui_zh_CN.ts WORKING_DIRECTORY ${I18N_DIR})
    execute_process(COMMAND ${QT_LRELEASE} fluentui_zh_CN.ts WORKING_DIRECTORY ${I18N_DIR})
endif ()

file(GLOB QM_FILES ${I18N_DIR}/ *.qm)
file(COPY ${QM_FILES} DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/qml/i18n")

#遍历所有Cpp文件
file(GLOB_RECURSE CPP_FILES *.cpp *.h *.cxx)
foreach (filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sources_files ${filename})
endforeach (filepath)

list(REMOVE_ITEM sources_files qhotkey/qhotkey_mac.cpp qhotkey/qhotkey_win.cpp qhotkey/qhotkey_x11.cpp)

if (WIN32)
    list(APPEND sources_files qhotkey/qhotkey_win.cpp)
elseif (APPLE)
    list(APPEND sources_files qhotkey/qhotkey_mac.cpp)
elseif (ANDROID)
    list(REMOVE_ITEM sources_files FluHotkey.cpp FluHotkey.h qhotkey/qhotkey.cpp qhotkey/qhotkey.h qhotkey/qhotkey_p.h)
elseif (UNIX)
    list(APPEND sources_files qhotkey/qhotkey_x11.cpp)
endif()

if (WIN32)
    set(FLUENTUI_VERSION_RC_PATH ${CMAKE_CURRENT_BINARY_DIR}/version_${PROJECT_NAME}.rc)
    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/version_dll.rc.in
            ${FLUENTUI_VERSION_RC_PATH}
    )
endif ()


if (NOT FLUENTUI_BUILD_STATIC_LIB)
    list(REMOVE_ITEM sources_files FluentUI.h FluentUI.cpp)
endif ()

#遍历所有qml文件

set(FLUENTUI_QML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qml")

# QML 文件（只收集 qml/ 下）
file(GLOB_RECURSE QML_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/**/qmldir"
)
foreach (filepath ${QML_PATHS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename "${filepath}")
    if (filename MATCHES "qmldir$")
        list(APPEND resource_files "${filename}")
    else ()
        list(APPEND qml_files "${filename}")
    endif ()
endforeach ()

# 资源文件（只收集 qml/ 下，包含 mjs）
file(GLOB_RECURSE RES_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.jpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.ico"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.ttf"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.webp"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.js"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.mjs"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qm"
)
foreach (filepath ${RES_PATHS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename "${filepath}")
    list(APPEND resource_files "${filename}")
endforeach ()


if (FLUENTUI_BUILD_STATIC_LIB)
    set(LIB_TYPE "STATIC")
else ()
    set(LIB_TYPE "SHARED")
endif ()

if (FLUENTUI_BUILD_STATIC_LIB)
    set(PLUGIN_TARGET_NAME "")
else ()
    #如果是动态库，则使用插件目标作为其自己的支持目标来定义 QML 模块，在这种情况下，模块必须在运行时动态加载，并且不能由其他目标直接链接到
    set(PLUGIN_TARGET_NAME ${PROJECT_NAME})
endif ()

#如果是Windows平台，则生成rc文件
set(FLUENTUI_VERSION_RC_PATH "")
if (WIN32)
    set(FLUENTUI_VERSION_RC_PATH ${CMAKE_CURRENT_BINARY_DIR}/version_${PROJECT_NAME}.rc)
    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/version_dll.rc.in
            ${FLUENTUI_VERSION_RC_PATH}
    )
endif ()

qt_add_library(${PROJECT_NAME} ${LIB_TYPE})
qt_add_qml_module(${PROJECT_NAME}
        PLUGIN_TARGET ${PLUGIN_TARGET_NAME}
        OUTPUT_DIRECTORY ${FLUENTUI_QML_PLUGIN_DIRECTORY}
        VERSION 1.0
        URI "FluentUI"
        #修改qmltypes文件名称。默认fluentuiplugin.qmltypes，使用默认名称有时候import FluentUI 1.0会爆红，所以修改成plugins.qmltypes
        TYPEINFO "plugins.qmltypes"
        SOURCES ${sources_files} ${FLUENTUI_VERSION_RC_PATH}
        QML_FILES ${qml_files}
        RESOURCES ${resource_files}
        RESOURCE_PREFIX "/qt/qml"
)

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        #导入qrcode配置文件
        HAVE_CONFIG_H
)

#去掉mingw生成的动态库libxxx前缀lib，不去掉前缀会导致 module "FluentUI" plugin "fluentuiplugin" not found
if (MINGW)
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
    #解决编译器报 too many sections的问题
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
endif ()

#MSVC Debug 添加后缀d，与Qt插件风格保持一致
if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC include)
#链接库
target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Widgets
        Qt6::PrintSupport
)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE Carbon)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qmlcustomplot
)
    
#安装
install(DIRECTORY ${FLUENTUI_QML_PLUGIN_DIRECTORY} DESTINATION ${CMAKE_INSTALL_PREFIX})

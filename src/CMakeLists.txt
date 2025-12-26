cmake_minimum_required(VERSION 3.21)

set(VERSION "1.0.0")

project(FMusic VERSION ${VERSION} LANGUAGES CXX)

# 1. 【关键修正】添加了 Widgets 和 PrintSupport (FluentUI 需要它们)
find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia Network Sql LinguistTools Widgets PrintSupport)

qt_standard_project_setup()

set(APPLICATION_DIR_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Option to enable SMTC support
option(ENABLE_SMTC "Enable SMTC support" OFF)

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

#设置信号与槽的控制类
file(GLOB_RECURSE HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/control/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/decrypt/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/db/*.h
)

# 2. 【关键修正】去掉了错误的 INSTALL_HEADERS 关键字
# 直接把 headers 放在列表里，IDE 就能识别了
qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${HEADERS}
)

# 3. 【关键修正】链接库中添加了 Qt6::Widgets 和 Qt6::PrintSupport
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Network
        Qt6::Multimedia
        Qt6::Sql
        Qt6::Widgets       # FluentUI 依赖
        Qt6::PrintSupport  # FluentUI 依赖
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

# 1. 定义 .ts 文件
file(GLOB TS_FILES "i18n/*.ts")

# 2. 【关键】设定输出路径
# 这里使用变量拼接，适配 Debug/Release。如果你只想死磕 Debug，可以写死 "bin/Debug/i18n"
set(I18N_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/i18n")

# 3. 为每个 .ts 文件设置 OUTPUT_LOCATION 属性
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

qt_finalize_executable(${PROJECT_NAME})
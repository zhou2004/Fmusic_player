cmake_minimum_required(VERSION 3.20)

project(Fmusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

if(APPLE)
    set(APPLICATION_DIR_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/MacOS)
else()
    set(APPLICATION_DIR_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# 查找 zlib 库
find_package(ZLIB REQUIRED)


#寻找taglib依赖包
find_package(taglib CONFIG REQUIRED)

find_package(kissfft CONFIG REQUIRED)

#寻找qt的依赖包
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Multimedia Bluetooth Network Gui Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Multimedia Bluetooth Network Gui Widgets Sql)

find_program(QT_LUPDATE NAMES lupdate)
find_program(QT_LRELEASE NAMES lrelease)

# file(GLOB TS_FILE_PATHS ${CMAKE_CURRENT_LIST_DIR}/ *.ts)
# add_custom_target(Script-UpdateTranslations
#     COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#     COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#     COMMAND ${QT_LRELEASE} ${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#     COMMAND ${QT_LRELEASE} ${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${APPLICATION_DIR_PATH}/i18n
#     COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}_en_US.qm ${PROJECT_NAME}_zh_CN.qm ${APPLICATION_DIR_PATH}/i18n
#     SOURCES ${TS_FILE_PATHS}
# )

set(PROJECT_SOURCES
        cpp/*.cpp
#        cpp/MusicPlayer.cpp
        qml.qrc
)

 file(GLOB PROJECT_SOURCES
         ${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cpp
         qml.qrc
 )

 #设置信号与槽的控制类
 file(GLOB_RECURSE HEADERS
         ${CMAKE_CURRENT_SOURCE_DIR}/include/control/*.h
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Decryptor/*.h
         ${CMAKE_CURRENT_SOURCE_DIR}/include/db/*.h
 )

 list(REMOVE_ITEM HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Decryptor/KRCDecryptor.h
 )

 qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

 qt_add_executable(${PROJECT_NAME}
         MANUAL_FINALIZATION
         ${PROJECT_SOURCES}
         ${MOC_SOURCES}
         resource.rc
 )

 target_link_libraries(${PROJECT_NAME} PRIVATE

        # Qt库
        Qt6::Core
        Qt${QT_VERSION_MAJOR}::Quick
        Qt6::Network
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::Sql
        # 建议显式加上 PrintSupport 以消除警告，虽然 fluentuiplugin 可能已经传递了它
        # Qt${QT_VERSION_MAJOR}::PrintSupport 
        fluentuiplugin

        # 2. 修正 taglib 大小写 (Conan 生成的是小写)
        taglib::tag
        # taglib::tag_c  <-- 如果不写 C 代码，这行可以注释掉

        # 3. 修正 kissfft (根据日志，Conan 只声明了 kissfft::kissfft)
        kissfft::kissfft

        ${ZLIB_LIBRARIES}
)


  target_include_directories(${PROJECT_NAME} PRIVATE
          ${CMAKE_CURRENT_SOURCE_DIR}/include
          ${CMAKE_CURRENT_SOURCE_DIR}/include/control
          ${CMAKE_CURRENT_SOURCE_DIR}/include/db
 )





##生成版本信息头文件
set(HEADER_FILE_VERSION_PATH ${CMAKE_SOURCE_DIR}/src/Version.h)
configure_file(
        ${CMAKE_SOURCE_DIR}/.cmake/Version.h.in
        ${HEADER_FILE_VERSION_PATH}
)

#如果是Windows平台，则生成rc文件，还有inno setup脚本文件
set(EXAMPLE_VERSION_RC_PATH "")
if (WIN32)
    set(EXAMPLE_VERSION_RC_PATH ${CMAKE_CURRENT_BINARY_DIR}/version_${PROJECT_NAME}.rc)
    configure_file(
            ${CMAKE_SOURCE_DIR}/.cmake/version_exe.rc.in
            ${EXAMPLE_VERSION_RC_PATH}
    )
    configure_file(
            ${CMAKE_SOURCE_DIR}/.cmake/InstallerScript.iss.in
            ${CMAKE_SOURCE_DIR}/package/InstallerScript.iss
    )
endif ()



set_target_properties(${PROJECT_NAME} PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})


qt_import_qml_plugins(${PROJECT_NAME})
qt_finalize_executable(${PROJECT_NAME})

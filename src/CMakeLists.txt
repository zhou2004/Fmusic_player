cmake_minimum_required(VERSION 3.20)

project(nice VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

if(APPLE)
    set(APPLICATION_DIR_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/MacOS)
else()
    set(APPLICATION_DIR_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# 查找 zlib 库
find_package(ZLIB REQUIRED)

# 查找 OpenSSL
find_package(OpenSSL REQUIRED)

#寻找rapidjson库  json解析器
find_package(rapidjson CONFIG REQUIRED)

#寻找taglib依赖包
find_package(taglib CONFIG REQUIRED)

find_package(kissfft CONFIG REQUIRED)
#寻找qt的依赖包
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Multimedia Bluetooth Network Gui Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Multimedia Bluetooth Network Gui Widgets Sql)

find_program(QT_LUPDATE NAMES lupdate)
find_program(QT_LRELEASE NAMES lrelease)
file(GLOB TS_FILE_PATHS ${CMAKE_CURRENT_LIST_DIR}/ *.ts)
add_custom_target(Script-UpdateTranslations
    COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${QT_LRELEASE} ${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${QT_LRELEASE} ${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPLICATION_DIR_PATH}/i18n
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}_en_US.qm ${PROJECT_NAME}_zh_CN.qm ${APPLICATION_DIR_PATH}/i18n
    SOURCES ${TS_FILE_PATHS}
)

#set(PROJECT_SOURCES
#        cpp/*.cpp
##        cpp/MusicPlayer.cpp
#        qml.qrc
#)

file(GLOB PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cpp
        qml.qrc
)

#设置信号与槽的控制类
file(GLOB_RECURSE HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/control/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Decryptor/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Sql/*.h
)


#set(HEADERS
#        include/control/MyController.h
#        include/control/MusicPlayer.h
#)

qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
            ${MOC_SOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
            ${MOC_SOURCES}
    )
endif()

add_dependencies(${PROJECT_NAME} Script-UpdateTranslations)

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto

        #Qt库
        Qt6::Core
        Qt${QT_VERSION_MAJOR}::Quick
        Qt6::Gui
        Qt6::Network
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::Bluetooth
        Qt${QT_VERSION_MAJOR}::Sql
        Qt6::Widgets
        fluentuiplugin


        #taglib库
        TagLib::tag
        TagLib::tag_c
        TagLib::TagLib


        #rapidjson库
        rapidjson

        kissfft::kissfft-float
        kissfft::kissfft-double

        ${ZLIB_LIBRARIES}

)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/control
)










if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()
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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()

if (CMAKE_BUILD_TYPE MATCHES "Release")
    if (APPLE)
        find_program(QT_DEPLOY_QT NAMES macdeployqt)
        add_custom_target(Script-DeployRelease
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/bin/
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_SOURCE_DIR}/dist
            COMMAND ${QT_DEPLOY_QT} ${CMAKE_SOURCE_DIR}/dist/${PROJECT_NAME}.app -qmldir=${CMAKE_CURRENT_LIST_DIR}
            COMMENT "MacOs Deploying Qt Dependencies After Build........."
            SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
    if(WIN32)
        find_program(QT_DEPLOY_QT NAMES windeployqt)
        # 复制 DLL 文件
#        execute_process(COMMAND ${CMAKE_COMMAND} -E copy "D:/Qt/6.8.1/mingw_64/bin/*.dll" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qml
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins
#            COMMAND ${QT_DEPLOY_QT} --qmldir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qml --plugindir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins --no-translations --compiler-runtime $<TARGET_FILE:${PROJECT_NAME}>
            COMMAND ${QT_DEPLOY_QT} --qmldir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qml --plugindir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe

            COMMENT "Windows Deploying Qt Dependencies After Build........."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endif()
endif()

#-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic
#-DCMAKE_TOOLCHAIN_FILE=D:\CLion\vcpkg\scripts\buildsystems\vcpkg.cmake
#-DVCPKG_INSTALLED_DIR=D:\pycharm\pythonProject\Fmusic_player\vcpkg_installed
#-DVCPKG_MANIFEST_INSTALL=OFF
#-DVCPKG_MANIFEST_MODE=OFF



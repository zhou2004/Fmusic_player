cmake_minimum_required(VERSION 3.20)

project(Fmusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APPLICATION_DIR_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# 查找 OpenSSL 库
find_package(OPENSSL REQUIRED)

# 查找 zlib 库
find_package(ZLIB REQUIRED)


#寻找taglib依赖包
find_package(taglib CONFIG REQUIRED)

find_package(kissfft CONFIG REQUIRED)

#寻找qt的依赖包
find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia Network Sql)

# 国际化支持
find_program(QT_LUPDATE NAMES lupdate lupdate-qt6)
find_program(QT_LRELEASE NAMES lrelease lrelease-qt6)
set(I18N_DIR "${CMAKE_CURRENT_LIST_DIR}/i18n")
file(GLOB TS_FILE_PATHS ${I18N_DIR}/*.ts)
add_custom_target(Script-UpdateTranslations
    COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${I18N_DIR}/${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${I18N_DIR}
    COMMAND ${QT_LUPDATE} ${CMAKE_CURRENT_LIST_DIR} -ts ${I18N_DIR}/${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${I18N_DIR}
    COMMAND ${QT_LRELEASE} ${I18N_DIR}/${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${I18N_DIR}
    COMMAND ${QT_LRELEASE} ${I18N_DIR}/${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${I18N_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPLICATION_DIR_PATH}/i18n
    COMMAND ${CMAKE_COMMAND} -E copy ${I18N_DIR}/${PROJECT_NAME}_en_US.qm ${I18N_DIR}/${PROJECT_NAME}_zh_CN.qm ${APPLICATION_DIR_PATH}/i18n
    SOURCES ${TS_FILE_PATHS}
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
        Qt6::Core
        Qt6::Quick
        Qt6::Network
        Qt6::Multimedia
        Qt6::Sql
        fluentuiplugin
        taglib::tag
        # taglib::tag_c  <-- 如果不写 C 代码，这行可以注释掉

         OpenSSL::Crypto
         OpenSSL::SSL

        # 3. 修正 kissfft (根据日志，Conan 只声明了 kissfft::kissfft)
        kissfft::kissfft
        ZLIB::ZLIB
)


  target_include_directories(${PROJECT_NAME} PRIVATE
          ${CMAKE_CURRENT_SOURCE_DIR}/include
          ${CMAKE_CURRENT_SOURCE_DIR}/include/control
          ${CMAKE_CURRENT_SOURCE_DIR}/include/Decryptor
          ${CMAKE_CURRENT_SOURCE_DIR}/include/db
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



if (WIN32)
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
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_import_qml_plugins(${PROJECT_NAME})
qt_finalize_executable(${PROJECT_NAME})

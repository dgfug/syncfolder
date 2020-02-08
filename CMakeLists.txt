cmake_minimum_required(VERSION 3.1.0)

project(syncfolder)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(Qt5_DIR "/Users/faywong/tools/Qt5.14.1/5.14.1/clang_64/lib/cmake/Qt5")
    message( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )
    message( STATUS "Qt5_DIR: " ${Qt5_DIR} )
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

add_executable(SyncFolder
    FileIconProvider.cpp
    FileLocatorWindow.cpp
    FullTextSearchWindow.cpp
    fileformat.cpp
    main.cpp
    mainwindow.cpp
    settingdialog.cpp
    syncapp.cpp
    configurationpage.cpp
    uiwidgets/ProgressCircle.cpp
    uiwidgets/DisplayQueuedFilesAction.cpp
    uiwidgets/MarkdownPreviewView.cpp
    uiwidgets/MarkdownPreviewView.h
    resources.qrc
    translations/syncfolder_zh_CN.ts
    qmarkdowntextedit/highlighter.cpp
    qmarkdowntextedit/pmh_parser.c
    qmarkdowntextedit/pmh_styleparser.c
    qmarkdowntextedit/qmarkdowntextedit.cpp
    qmarkdowntextedit/qplaintexteditsearchwidget.cpp
    qmarkdowntextedit/qplaintexteditsearchwidget.ui
    qmarkdowntextedit/media.qrc
    qmarkdowntextedit/LineNumberArea.cpp
    qmarkdowntextedit/LineNumberArea.h
    3rdparty/QSimpleUpdater/src/Updater.cpp
    3rdparty/QSimpleUpdater/src/Updater.h
    3rdparty/QSimpleUpdater/src/Downloader.cpp
    3rdparty/QSimpleUpdater/src/Downloader.h
    3rdparty/QSimpleUpdater/src/QSimpleUpdater.cpp
    3rdparty/QSimpleUpdater/include/QSimpleUpdater.h
    3rdparty/QSimpleUpdater/src/Downloader.ui
    3rdparty/QSimpleUpdater/etc/resources/qsimpleupdater.qrc
        ISettingPage.h)

target_include_directories(SyncFolder PUBLIC
        .
        uiwidgets
        qmarkdowntextedit
        3rdparty/QSimpleUpdater
        3rdparty/QSimpleUpdater/include
        3rdparty/QSimpleUpdater/src)

set_target_properties(SyncFolder PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

target_link_libraries(SyncFolder Qt5::Widgets Qt5::Concurrent Qt5::Network)

#add_subdirectory(3rdparty/litehtml-qt/)

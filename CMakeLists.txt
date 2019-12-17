cmake_minimum_required(VERSION 3.1.0)

project(syncfolder)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

message( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )
message( STATUS "Qt5_DIR: " ${Qt5_DIR} )

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)
find_package(Qt5AutoUpdaterCore REQUIRED)
find_package(Qt5AutoUpdaterWidgets REQUIRED)

#find_package( QtAutoUpdater REQUIRED )

add_executable(SyncFolder
    FileIconProvider.cpp
    FileLocatorWindow.cpp
    FullTextSearchWindow.cpp
    configurationpage.cpp
    fileformat.cpp
    main.cpp
    mainwindow.cpp
    settingdialog.cpp
    syncapp.cpp
    uiwidgets/ProgressCircle.cpp
    resources.qrc
    translations/syncfolder_zh_CN.ts
    qmarkdowntextedit/highlighter.cpp
    qmarkdowntextedit/pmh_parser.c
    qmarkdowntextedit/pmh_styleparser.c
    qmarkdowntextedit/qmarkdowntextedit.cpp
    qmarkdowntextedit/qplaintexteditsearchwidget.cpp
    qmarkdowntextedit/qplaintexteditsearchwidget.ui
    qmarkdowntextedit/media.qrc
)

target_include_directories(SyncFolder PUBLIC
        .
        uiwidgets
        qmarkdowntextedit)

set_target_properties(SyncFolder PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

target_link_libraries(SyncFolder Qt5::Widgets Qt5::Concurrent Qt5AutoUpdaterCore Qt5AutoUpdaterWidgets)

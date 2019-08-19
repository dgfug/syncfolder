#-------------------------------------------------
#
# Project created by QtCreator 2019-08-02T14:20:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SyncFolder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        FileIconProvider.cpp \
        FileLocatorWindow.cpp \
        FullTextSearchWindow.cpp \
        main.cpp \
        mainwindow.cpp \

INCLUDEPATH += . \

HEADERS += \
        EditorDelegate.h \
        FileIconProvider.h \
        FileLocatorWindow.h \
        FullTextSearchWindow.h \
        mainwindow.h \
        mainwindowbuilder.h \
        qmarkdowntextedit/highlighter.h \
        qmarkdowntextedit/pmh_definitions.h \
        qmarkdowntextedit/pmh_parser.h \
        qmarkdowntextedit/pmh_styleparser.h \
        settings/settings_def.h

FORMS +=

include(qmarkdowntextedit/qmarkdowntextedit.pri)

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#-------------------------------------------------
#
# Project created by QtCreator 2019-08-02T14:20:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DMEditor
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

CONFIG += c++11

SOURCES += \
        dmfileiconprovider.cpp \
        findfilewindow.cpp \
        fulltextsearchwindow.cpp \
        main.cpp \
        mainwindow.cpp \
        ag_2_2_0/src/ignore.c \
        ag_2_2_0/src/options.c \
        ag_2_2_0/src/print.c \
        ag_2_2_0/src/print_w32.c \
        ag_2_2_0/src/scandir.c \
        ag_2_2_0/src/search.c \
        ag_2_2_0/src/lang.c \
        ag_2_2_0/src/util.c \
        ag_2_2_0/src/decompress.c \
        ag_2_2_0/src/zfile.c \
        ag_2_2_0/src/log.c \

INCLUDEPATH += . \
               ag_2_2_0 \

HEADERS += \
        dmfileiconprovider.h \
        editerdelegate.h \
        findfilewindow.h \
        fulltextsearchwidow.h \
        mainwindow.h \
        qmarkdowntextedit/highlighter.h \
        qmarkdowntextedit/pmh_definitions.h \
        qmarkdowntextedit/pmh_parser.h \
        qmarkdowntextedit/pmh_styleparser.h \
        settings/settings_def.h \
        ui_mainwindow_helper.h

FORMS +=

include(qmarkdowntextedit/qmarkdowntextedit.pri)

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

macx: LIBS += -L$$PWD/../../../../usr/local/Cellar/pcre/8.43/lib/ -lpcre

INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/pcre/8.43/include
DEPENDPATH += $$PWD/../../../../usr/local/Cellar/pcre/8.43/include

macx: PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/pcre/8.43/lib/libpcre.a

macx: LIBS += -L$$PWD/../../../../usr/local/Cellar/xz/5.2.4/lib/ -llzma

INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/xz/5.2.4/include
DEPENDPATH += $$PWD/../../../../usr/local/Cellar/xz/5.2.4/include

macx: PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/xz/5.2.4/lib/liblzma.a

macx: LIBS += -L$$PWD/../../../../usr/local/Cellar/zlib/1.2.11/lib/ -lz

INCLUDEPATH += $$PWD/../../../../usr/local/Cellar/zlib/1.2.11/include
DEPENDPATH += $$PWD/../../../../usr/local/Cellar/zlib/1.2.11/include

macx: PRE_TARGETDEPS += $$PWD/../../../../usr/local/Cellar/zlib/1.2.11/lib/libz.a

#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <qsettings.h>
#include <QDebug>
#include <QStyleFactory>
#include <QtGlobal>
#include <QTranslator>
#include "settings/settings_def.h"
#include "syncapp.h"

int main(int argc, char *argv[])
{
    SyncApp a(argc, argv);
    a.setStyleSheet("QStatusBar, QWidget[objectName^=\"centralWidget\"] { background-color: #F7F6F8; }");
    QTranslator translator;
    QString locale = QLocale::system().name();
    if (locale == "zh_CN") {
        translator.load(":/translations/syncfolder_zh_CN.qm");
    }
    a.installTranslator(&translator);

#if defined (Q_OS_MAC) || defined (Q_OS_MACOS)
    if (QStyleFactory::keys().contains("macintosh")) {
        a.setStyle(QStyleFactory::create("macintosh"));
    }
#elif defined (Q_OS_LINUX)
    if (QStyleFactory::keys().contains("kvantum")) {
        a.setStyle(QStyleFactory::create("kvantum"));
        qDebug() <<"kvantum";
    }
#endif

    QCoreApplication::setApplicationVersion(SYNC_FOLDER_VER);
    QCoreApplication::setOrganizationName("faywong personal");
    QCoreApplication::setOrganizationDomain("syncfolder.com");
    QCoreApplication::setApplicationName("SyncFolder");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCommandLineParser parser;
    parser.setApplicationDescription("SyncFolder");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("directory", "The directory to start in.");
    parser.process(a);

    QString rootPath = parser.positionalArguments().isEmpty()
        ? QString() : parser.positionalArguments().first();

    MainWindow w;

    if (rootPath.isEmpty()) {
        rootPath = SyncFolderSettings::getString(KEY_LAST_FOLDER);
    }

    if (!rootPath.isEmpty()) {
        w.setCurrentRootDirPath(rootPath);
    }

    QString lastEditedFile = SyncFolderSettings::getString(KEY_LAST_FILE);
    if (!lastEditedFile.isEmpty()) {
        w.openFile_l(lastEditedFile, 1, true);
    }

    QObject::connect(&a, SIGNAL(urlOpened(const QString &)), &w, SLOT(handleOrgCaptured(const QString &)));

    w.show();

    return a.exec();
}

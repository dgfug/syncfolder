#include "mainwindow.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QStyleFactory>
#include <QtGlobal>
#include <QTranslator>
#include "settings/settings_def.h"
#include "syncapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    SyncApp a(argc, argv);
    a.setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                      "    background:white;"
                                      "    width: 8px;    "
                                      "    margin: 0px 0px 0px 0px;"
                                      "}"
                                      "QScrollBar::handle:vertical {"
                                      "    background: #C2C2C2;"
                                      "    min-height: 0px;"
                                      "    border: 1px transparent #2A2929;\n"
                                      "    border-radius: 3px;"
                                      "}"
                                      "QScrollBar::add-line:vertical {"
                                      "    background: #C2C2C2;"
                                      "    border: 1px transparent #2A2929;\n"
                                      "    border-radius: 3px;"
                                      "    height: 0px;"
                                      "    subcontrol-position: bottom;"
                                      "    subcontrol-origin: margin;"
                                      "}"
                                      "QScrollBar::sub-line:vertical {"
                                      "    background: #C2C2C2;"
                                      "    height: 0 px;"
                                      "    subcontrol-position: top;"
                                      "    subcontrol-origin: margin;"
                                      "}"
                                      "QStatusBar, QWidget[objectName^=\"centralWidget\"] {"
                                      "    background-color: #F7F6F8;"
                                      "}"
    ));
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

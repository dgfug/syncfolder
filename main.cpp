#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <qsettings.h>
#include "settings/settings_def.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCoreApplication::setOrganizationName("faywong personal");
    QCoreApplication::setOrganizationDomain("dmeditor.com");
    QCoreApplication::setApplicationName("DMEditor");
    QCommandLineParser parser;
    parser.setApplicationDescription("DMEditor");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("directory", "The directory to start in.");
    parser.process(a);
    QString rootPath = parser.positionalArguments().isEmpty()
        ? QString() : parser.positionalArguments().first();

    MainWindow w;
    if (rootPath.isEmpty()) {
        rootPath = DMSettings::getString(KEY_LAST_FOLDER);
    }

    if (!rootPath.isEmpty()) {
        w.setCurrentRootDirPath(rootPath);
    }

    w.show();

    return a.exec();
}

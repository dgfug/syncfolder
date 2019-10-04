#include "syncapp.h"

SyncApp::SyncApp(int &argc, char **argv)
    : QApplication(argc, argv)
{
}

SyncApp::~SyncApp() {

}
bool SyncApp::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        if (!openEvent->url().isEmpty())
        {
            auto url = openEvent->url().toString();
            emit urlOpened(url);
            QMessageBox msgBox;
            msgBox.setText("org protocol");
            msgBox.setInformativeText(url);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            return true;
        }
    }

    return QApplication::event(event);
}


#ifndef SYNCAPP_H
#define SYNCAPP_H

#include <QApplication>
#include <QFileOpenEvent>
#include <QMessageBox>
#include <QtDebug>
#define SYNC_FOLDER_VER "0.1.0"

class SyncApp : public QApplication
{
    Q_OBJECT

signals:
    void urlOpened(const QString &url);
public:
    SyncApp(int &argc, char **argv);

    virtual ~SyncApp() override;

    bool event(QEvent *event) override;
};
#endif // SYNCAPP_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QItemSelection>
#include <QMainWindow>
#include "fulltextsearchwidow.h"
#include "findfilewindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public DMEditorDelegate
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, QString* dirPath = nullptr);
    ~MainWindow() override;

    void setCurrentRootDirPath(const QString &value);

    void openFile_l(const QString &, size_t lineNo) override;

public slots:
    void newFile();
    void openFile();
    void openDirectory();
    void saveFile();
    void fileSelectionChanged(const QItemSelection&,const QItemSelection&);
    void launchSearchWindow();
    void launchFindFileWindow();

private:
    Ui::MainWindow *ui;
    void setupFileMenu();
    QString currentFilePath;
    QString currentRootDirPath;
    std::vector<std::string> fileNamesDictionary;
    FullTextSearchWindow *searchWindow;
    FindFileWindow *findFileWindow;
};

#endif // MAINWINDOW_H

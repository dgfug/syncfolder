#include "mainwindow.h"
#include "ui_mainwindow_helper.h"
#include "qmarkdowntextedit.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include "settings/settings_def.h"
#include <QStandardPaths>
#include <iostream>
#include <string>
#include <QTextBlock>

MainWindow::MainWindow(QWidget *parent, QString* dirPath) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentFilePath(""),
    currentRootDirPath(""),
    searchWindow(nullptr),
    findFileWindow(nullptr),
    detailsLabel(nullptr),
    syncDetailsIcon(nullptr)
{
    ui->setupUi(this);
    setupFileMenu();
}

void MainWindow::fileSelectionChanged(const QItemSelection& selected,const QItemSelection& deselected) {
    QModelIndexList selectedList = selected.indexes();
    if (!selectedList.empty()) {
        auto modelIndex = selectedList[0];
        if (!ui->fileTreeModel->isDir(modelIndex)) {
            QString path = ui->fileTreeModel->filePath(selectedList[0]);
            openFile_l(path, 1);
        }
    }
}

void MainWindow::openFile_l(const QString &filePath, size_t lineNo, bool needSelect) {
    currentFilePath = filePath;
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists()) {
            return;
        }
        QFile file(filePath);

        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream fileToRead(&file);
        ui->markdownEditor->setText(fileToRead.readAll());
        QTextCursor cursor(ui->markdownEditor->document()->findBlockByLineNumber(lineNo - 1));
        ui->markdownEditor->moveCursor(QTextCursor::End);
        ui->markdownEditor->setTextCursor(cursor);
        setWindowTitle(QCoreApplication::translate("MainWindow", fileInfo.fileName().toStdString().c_str(), nullptr));
        DMSettings::setString(KEY_LAST_FILE, filePath);
        if (needSelect) {
            selectInFolderView();
        }
    }
}

void MainWindow::setCurrentRootDirPath(const QString &folderPath)
{
    currentRootDirPath = folderPath;
    if (!folderPath.isEmpty()) {
        ui->fileTreeModel->setRootPath(folderPath);
        const QModelIndex rootIndex = ui->fileTreeModel->index(QDir::cleanPath(folderPath));
        if (rootIndex.isValid())
            ui->fileTree->setRootIndex(rootIndex);

        DMSettings::setString(KEY_LAST_FOLDER, folderPath);
    }

    QDirIterator it(currentRootDirPath, QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);

    fileNamesDictionary.clear();
    while (it.hasNext()) {
        QString dirName = it.next();
        std::string utf8_text = dirName.toUtf8().constData();
        fileNamesDictionary.push_back(utf8_text);
    }
}

void MainWindow::contextMenu(const QPoint &pos) {
//    const QTableWidgetItem *item = ui->fileTree->itemAt(pos);
//    if (!item)
//        return;
//    QMenu menu;
//#ifndef QT_NO_CLIPBOARD
//    QAction *copyAction = menu.addAction("Copy Name");
//#endif
//    QAction *openAction = menu.addAction("Open");
//    QAction *action = menu.exec(filesTable->mapToGlobal(pos));
//    if (!action)
//        return;
//    const QString fileName = fileNameOfItem(item);
//    if (action == openAction)
//        openFile(fileName);
//#ifndef QT_NO_CLIPBOARD
//    else if (action == copyAction)
//        QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(fileName));
    //#endif
}

void MainWindow::processStdOutput()
{
    syncLog += unisonProcess->readAllStandardOutput();  // read error channel
}

void MainWindow::processStdError()
{
    syncLog += unisonProcess->readAllStandardError();  // read error channel
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
            tr("Open File"), "", "Markdown Files (*.mdtext *.md *.markdown *.txt *.text)");

    if (!filePath.isEmpty()) {
        QFileInfo checkFile(filePath);
        if (checkFile.isDir()) {
            setCurrentRootDirPath(filePath);
        } else if (checkFile.isFile()) {
            openFile_l(filePath, 1);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Error");
            msgBox.setInformativeText("file at path: " + filePath + " is not able to be edited!");
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
    }
}

void MainWindow::openDirectory()
{
    QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    homeLocation,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dirPath.isEmpty()) {
        QFileInfo checkFile(dirPath);
        if (checkFile.isDir()) {
            setCurrentRootDirPath(dirPath);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Error");
            msgBox.setInformativeText("file at path: " + dirPath + " is not able to be edited!");
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
    }
}

void MainWindow::newFile()
{
    auto selectionIndex = ui->fileTree->selectionModel()->currentIndex();
    auto selectedFile = ui->fileTreeModel->fileInfo(selectionIndex);

    // TODO refine this
    if (selectedFile.isDir()) {
        ui->fileTreeModel->mkdir(selectionIndex, "untitled.md");
    } else {
        auto dirPath = selectedFile.filePath();
    }
}

void MainWindow::saveFile()
{
    if (!currentFilePath.isEmpty()) {
        QFile file(currentFilePath);
        file.open(QFile::WriteOnly | QFile::Text);
        QTextStream fileToWrite(&file);
        fileToWrite << ui->markdownEditor->toPlainText();
        file.flush();
        file.close();
    }
}

void MainWindow::syncFiles() {
    syncLog.clear();
    unisonProcess = new QProcess(this);
    connect(unisonProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleSyncFinished(int, QProcess::ExitStatus)));
    connect(unisonProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdOutput()));  // connect process signals with your code
    connect(unisonProcess, SIGNAL(readyReadStandardError()), this, SLOT(processStdError()));  // same here
    QString command(QString("/Users/faywong/bin/unison %1 %2").arg("default", "-batch"));
//    qDebug() << "About to invoke" << command;
    unisonProcess->start(command);
}

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
                        QKeySequence::New);

    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
                        QKeySequence::Open);

    fileMenu->addAction(tr("&Open dir..."), this, SLOT(openDirectory()),
                        QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_N));

    fileMenu->addAction(tr("&Save..."), this, SLOT(saveFile()),
                        QKeySequence::Save);

    fileMenu->addAction(tr("&Sync"), this, SLOT(syncFiles()),
                        QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_S));

    fileMenu->addAction(tr("&Select in Folder View"), this, SLOT(selectInFolderView()),
                        QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_J));

    fileMenu->addAction(tr("&Exit"), qApp, SLOT(quit()),
                        QKeySequence::Quit);

    QAction *launchSearchAction = new QAction(this);
    launchSearchAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_F);
    connect(launchSearchAction, SIGNAL(triggered()), this, SLOT(launchSearchWindow()));
    this->addAction(launchSearchAction);

    QAction *launchFindFileAction = new QAction(this);
    launchFindFileAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_O);
    connect(launchFindFileAction, SIGNAL(triggered()), this, SLOT(launchFindFileWindow()));
    this->addAction(launchFindFileAction);
}

void MainWindow::launchSearchWindow() {
    if (searchWindow == nullptr) {
        searchWindow = new FullTextSearchWindow(this);
    }
    searchWindow->show();
}

void MainWindow::launchFindFileWindow() {
    if (findFileWindow == nullptr) {
        findFileWindow = new FindFileWindow(this, this->fileNamesDictionary);
    }
    findFileWindow->show();
}

void MainWindow::handleSyncFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QString status;
    QString emoji;
    switch (exitCode) {
    case 0:
        status = "successful synchronization; everything is up-to-date now.";
        emoji = "😀";
        break;
    case 1:
        status = "some files were skipped, but all file transfers were successful.";
        emoji = "😞";
        break;
    case 2:
        status = "non-fatal failures occurred during file transfer.";
        emoji = "😥";
        break;
    case 3:
        status = "a fatal error occurred, or the execution was interrupted.";
        emoji = "😭";
        break;
    default:
        status = "unknown";
        emoji = "💥";
        break;
    }

    QString result = QString("%1 Sync result: %2, exit: %3").arg(emoji).arg(status).arg(exitCode);
    if (detailsLabel == nullptr) {
        detailsLabel = new QLabel(result,
                                          ui->statusBar);
        ui->statusBar->addWidget(detailsLabel);
    } else {
        detailsLabel->setText(result);
    }


    if (syncDetailsIcon == nullptr) {
        syncDetailsIcon = new QPushButton("", ui->statusBar);
        syncDetailsIcon->setIcon(QIcon(":/icons/details.svg"));
        syncDetailsIcon->setToolTip(tr("show details"));
        ui->statusBar->addWidget(syncDetailsIcon);
    }

    connect(syncDetailsIcon, SIGNAL(clicked(bool)), this, SLOT(showSyncDetails(bool)));
}

void MainWindow::showSyncDetails(bool checked) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("QMessageBox Example");
    msgBox.setText("details of file sync activity");
    msgBox.setInformativeText(syncLog);
    msgBox.exec();
    // TODO: guide user to check settings
}

void MainWindow::handleFileRenamed(const QString &path, const QString &oldName, const QString &newName) {
    setWindowTitle(QCoreApplication::translate("MainWindow", newName.toStdString().c_str(), nullptr));
}

void MainWindow::selectInFolderView() {
    if (!currentFilePath.isEmpty()) {
        auto index = ui->fileTreeModel->index(currentFilePath, 0);
        ui->fileTree->scrollTo(index);
        ui->fileTree->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

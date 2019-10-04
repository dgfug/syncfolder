#include "mainwindow.h"
#include "mainwindowbuilder.h"
#include "qmarkdowntextedit.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include "settings/settings_def.h"
#include <QStandardPaths>
#include <iostream>
#include <string>
#include <QTextBlock>
#include <QFile>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>
#include <fileformat.h>
#include <QMimeDatabase>
#include <QInputDialog>
#include <QUrlQuery>
#include <QUrl>
#include "syncapp.h"
#include <QDateTime>

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
    setAcceptDrops(true);
}

void MainWindow::handleOrgCaptured(const QString &url)
{
    QUrl note = QUrl::fromEncoded(url.toUtf8().constData());
    note.setQuery(note.query(QUrl::FullyDecoded), QUrl::DecodedMode);
    QUrlQuery urlQuery(note);
    QString u = urlQuery.queryItemValue("url");
    QString title = urlQuery.queryItemValue("title");
    QString body = urlQuery.queryItemValue("body");
    QString now = QDateTime::currentDateTime().toString("hh:mm:ss_yyyy-MM-dd");
    body = body.replace("%0A", "");
    newFileWithTitleContent(QString("%1_%2").arg(title).arg(now), body);
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
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            return;
        }
        currentFilePath = filePath;
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
            revealInFolderView();
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
    const QModelIndex index = ui->fileTree->indexAt(pos);
    const QString path = ui->fileTreeModel->filePath(index);
    QMenu menu;
    QAction *removeAction = menu.addAction("delete");
    QAction *createFolderUnderRoot = menu.addAction("create folder under root");
#ifndef QT_NO_CLIPBOARD
    QAction *copyAction = menu.addAction("copy path to clipboard");
#endif
    QAction *action = menu.exec(ui->fileTree->mapToGlobal(pos));
    if (!action)
        return;
    if (action == removeAction) {
        ui->fileTreeModel->remove(index);
    }
#ifndef QT_NO_CLIPBOARD
    else if (action == copyAction) {
        QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(path));
    }
#endif
    else if (action == createFolderUnderRoot) {
        auto index = ui->fileTreeModel->index(currentRootDirPath, 0);
        int i= 0;

        while (true) {
            QString name = (i == 0) ? QString("untitled") : QString("untitled_%1").arg(i);
            QFileInfo newFile = QFileInfo(currentRootDirPath, name);
            if (!newFile.exists()) {
                auto i = ui->fileTreeModel->mkdir(index, name);
                ui->fileTree->scrollTo(i);
                ui->fileTree->selectionModel()->select(i, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                break;
            }
            i++;
        }
    }
}

void MainWindow::processStdOutput()
{
//    qDebug()<< unisonProcess->readAllStandardOutput();
    syncProgressBar->setValue(syncProgressBar->value() + 4);
    syncLog += unisonProcess->readAllStandardOutput();  // read error channel
}

void MainWindow::processStdError()
{
//    qDebug()<< unisonProcess->readAllStandardError();
    syncProgressBar->setValue(1);
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

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
           event->acceptProposedAction();
  // if some actions should not be usable, like move, this code must be adopted
}

void MainWindow::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QMimeDatabase mimeDatabase;
        foreach (QUrl url, event->mimeData()->urls()) {
//            qDebug()<<"drop url: " << url;
            auto filePath = url.toLocalFile();
            QFileInfo fileInfo(filePath);
            if (fileInfo.isFile()) {
                const QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);
                if (FileFormat::getType(mimeType) == FileFormat::FMT::MarkDown) {
                    openFile_l(filePath, 1);
                }
            } else if (fileInfo.isDir()) {
                setCurrentRootDirPath(filePath);
            }
        }
    }
}

QFileInfo MainWindow::selectedFile() {
    auto selectionIndex = ui->fileTree->selectionModel()->currentIndex();
    return ui->fileTreeModel->fileInfo(selectionIndex);
}

void MainWindow::newFileWithTitleContent(const QString &title, const QString &content) {
    auto selectionIndex = ui->fileTree->selectionModel()->currentIndex();
    auto selectedFile = ui->fileTreeModel->fileInfo(selectionIndex);

    QString dir = selectedFile.isDir() ? selectedFile.absoluteFilePath() : selectedFile.absolutePath();
    int i= 0;

    while (true) {
        QString name = i == 0 ? QString("%1.md").arg(title) : QString("%1%2.md").arg(title).arg(i);
        QFileInfo newFile = QFileInfo(dir, name);
        if (!newFile.exists()) {
            QString filePath = newFile.absoluteFilePath();
            QFile n = QFile(filePath);
            n.open(QIODevice::WriteOnly | QIODevice::Append);
            if (!content.isEmpty()) {
                currentFilePath = filePath;
                saveFileFromText(content);
            }
            openFile_l(filePath, 1, true);
            ui->fileTree->edit(ui->fileTreeModel->index(filePath));
            break;
        }
        i++;
    }
}

void MainWindow::newFile()
{
    newFileWithTitleContent("untitled", "");
}

void MainWindow::saveFileFromText(const QString &text) {
    if (!currentFilePath.isEmpty()) {
        QFile file(currentFilePath);
        file.open(QFile::WriteOnly | QFile::Text);
        QTextStream fileToWrite(&file);
        fileToWrite << text;
        file.flush();
        file.close();
    }
}


void MainWindow::saveFile()
{
    saveFileFromText(ui->markdownEditor->toPlainText());
}

void MainWindow::syncFiles() {
    syncLog.clear();

    if (detailsLabel) {
        detailsLabel->setVisible(false);
    }

    if (syncDetailsIcon) {
        syncDetailsIcon->setVisible(false);
    }

    if (syncProgressBar == nullptr) {
        syncProgressBar = new QProgressBar();
        syncProgressBar->setRange(0, 100);
        syncProgressBar->setValue(10);
        syncProgressBar->setTextVisible(true);
        syncProgressBar->setFormat("Sync ongoing...");
    }

    syncProgressBar->setVisible(true);
    statusBar()->addWidget(syncProgressBar);

    unisonProcess = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString syncConfigDirPath = getSyncConfigDir();
    env.insert("UNISON", syncConfigDirPath); // Add an environment variable
    unisonProcess->setProcessEnvironment(env);
    connect(unisonProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleSyncFinished(int, QProcess::ExitStatus)));
    connect(unisonProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdOutput()));  // connect process signals with your code
    connect(unisonProcess, SIGNAL(readyReadStandardError()), this, SLOT(processStdError()));  // same here
    QString command(QString("/Users/faywong/bin/unison %1 %2").arg("default", "-batch"));
    unisonProcess->start(command);
}

QString MainWindow::getSyncConfigDir() {
    const QString configDirName = ".sync";
    QDir currentRootDir(currentRootDirPath);
    if (!currentRootDir.exists(configDirName)) {
        currentRootDir.mkdir(configDirName);
    }

    return currentRootDir.filePath(configDirName);
}

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
                        QKeySequence::New);

    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()));

    fileMenu->addAction(tr("&Open dir..."), this, SLOT(openDirectory()));

    fileMenu->addAction(tr("&Save..."), this, SLOT(saveFile()),
                        QKeySequence::Save);

    fileMenu->addAction(tr("&Sync"), this, SLOT(syncFiles()),
                        QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_S));

    fileMenu->addAction(tr("&Reveal in Folder View"), this, SLOT(revealInFolderView()),
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
        syncProgressBar->setValue(100);
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

    QString result = QString("%1 Sync result: %2, status: %3").arg(emoji).arg(status).arg(exitCode);

    if (detailsLabel == nullptr) {
        detailsLabel = new QLabel(result, ui->statusBar);
        ui->statusBar->addWidget(detailsLabel);
    } else {
        detailsLabel->setVisible(true);
        detailsLabel->setText(result);
    }

    if (syncDetailsIcon == nullptr) {
        syncDetailsIcon = new QPushButton("", ui->statusBar);
        syncDetailsIcon->setIcon(QIcon(":/icons/details.svg"));
        syncDetailsIcon->setToolTip(tr("show details"));
        ui->statusBar->addWidget(syncDetailsIcon);
    }
    syncProgressBar->setVisible(false);
    syncDetailsIcon->setVisible(exitCode != 0);

    connect(syncDetailsIcon, SIGNAL(clicked(bool)), this, SLOT(showSyncDetails(bool)));
}

void MainWindow::showSyncDetails(bool checked) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Sync details");
    msgBox.setText("sync failed, please check your sync settings");
    QRegularExpression re("^Unison.*SSH");
    re.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
    syncLog = syncLog.replace(re, "OpenSSH");
    msgBox.setDetailedText(syncLog);
    msgBox.exec();
}

void MainWindow::handleFileRenamed(const QString &path, const QString &oldName, const QString &newName) {
    setWindowTitle(QCoreApplication::translate("MainWindow", newName.toStdString().c_str(), nullptr));
    QFileInfo fileInfo(path, newName);
    openFile_l(fileInfo.filePath(), 1, true);
}

void MainWindow::revealInFolderView() {
    revealInFolderView_l(currentFilePath);
}

void MainWindow::revealInFolderView_l(const QString &path) {
    if (!path.isEmpty()) {
        auto index = ui->fileTreeModel->index(path, 0);
        ui->fileTree->scrollTo(index);
        ui->fileTree->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

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
#include <QImageReader>
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
#include "settingdialog.h"
#include <QtWidgets/QWidgetAction>
#include <QDesktopServices>
#include <QTimer>
#include "DisplayQueuedFilesAction.h"

MainWindow::MainWindow(QWidget *parent, QString* dirPath) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentFilePath(""),
    currentRootDirPath(""),
    searchWindow(nullptr),
    findFileWindow(nullptr)
{
    ui->setupUi(this);
    setupMenus();
    setAcceptDrops(true);
    /* QSimpleUpdater is single-instance */
    m_updater = QSimpleUpdater::getInstance();

    qint64 days = SyncFolderSettings::getDateTime(KEY_LAST_CHECK_UPDATE).daysTo(QDateTime::currentDateTime());
    if (qAbs(days) > 1) {
        /* Check for updates when the "Check For Updates" button is clicked */
        connect (m_updater, SIGNAL (checkingFinished  (QString)),
                 this,        SLOT (onCheckingUpdateFinished(QString)));
        connect (m_updater, SIGNAL (appcastDownloaded (QString, QByteArray)),
                 this,        SLOT (displayAppcast    (QString, QByteArray)));
        connect (m_updater, SIGNAL (downloadFinished (QString, QString)),
                 this,        SLOT (onUpdateDownloadFinished(QString, QString)));

        /* Apply the settings */
        m_updater->setModuleVersion (syncfolderUpdateUrl, SYNC_FOLDER_VER);
        m_updater->setNotifyOnFinish (syncfolderUpdateUrl, false);
        m_updater->setNotifyOnUpdate (syncfolderUpdateUrl, true);
        m_updater->setUseCustomAppcast (syncfolderUpdateUrl, false);
        m_updater->setDownloaderEnabled (syncfolderUpdateUrl, true);
        m_updater->setMandatoryUpdate (syncfolderUpdateUrl, true);

        /* Check for updates */
        m_updater->checkForUpdates (syncfolderUpdateUrl);
    }
}

void MainWindow::onCheckingUpdateFinished (const QString& url)
{
    // 确实有更新，则记下更新时间
    if (m_updater->getUpdateAvailable(url)) {
        SyncFolderSettings::setDateTime(KEY_LAST_CHECK_UPDATE, QDateTime::currentDateTime());
    }
//    qDebug()<<"changelog: "<<m_updater->getChangelog (url);
}

void MainWindow::displayAppcast(const QString& url, const QByteArray& reply)
{
    QString text = "This is the downloaded appcast: <p><pre>" +
                   QString::fromUtf8 (reply) +
                   "</pre></p><p> If you need to store more information on the "
                   "appcast (or use another format), just use the "
                   "<b>QSimpleUpdater::setCustomAppcast()</b> function. "
                   "It allows your application to interpret the appcast "
                   "using your code and not QSU's code.</p>";

    qDebug()<<"changelog: "<< text;
}

void MainWindow::onUpdateDownloadFinished(const QString& url, const QString& path)
{
    qDebug()<<"url: "<< url << ", path: " << path;
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

void MainWindow::handleTocClicked(const QItemSelection& selected,const QItemSelection& deselected) {
    QModelIndexList selectedList = selected.indexes();
    if (!selectedList.empty()) {
        auto modelIndex = selectedList[0];
        qlonglong pos = ui->tocModel->data(modelIndex, Qt::UserRole + 1).toLongLong();
        qDebug()<<"pos: "  + pos;
        ui->markdownEditor->jumpTo(pos);
    }
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
    const QByteArrayList supportedMimeTypes = QImageReader::supportedMimeTypes();
    QMimeDatabase mimeDatabase;
    QFileInfo fileInfo(filePath);
    const QMimeType mimeType = mimeDatabase.mimeTypeForFile(fileInfo);

    bool isImage = false;
    for (const QByteArray &mimeTypeName : supportedMimeTypes) {
        if (mimeType.inherits(mimeTypeName)) {
            isImage = true;
            break;
        }
    }

    if (isImage) { // image for read
        QImageReader reader(filePath);
        reader.setAutoTransform(true);
        const QImage newImage = reader.read();
        if (!newImage.isNull()) {
            ui->markdownEditor->setVisible(false);
            ui->imageScrollArea->setVisible(true);
            ui->splitter->setStretchFactor(0, 0);
            ui->imageLabel->setPixmap(QPixmap::fromImage(newImage));
        }
    } else if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            return;
        }
        currentFilePath = filePath;
        QFile file(filePath);

        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream fileToRead(&file);
        // TODO: move file io into non-main thread
        ui->markdownEditor->setVisible(true);
        ui->imageScrollArea->setVisible(false);
        ui->splitter->setStretchFactor(0, 1);
        ui->splitter->setStretchFactor(1, 3);

        QString text = fileToRead.readAll();
        ui->markdownEditor->setText(text);
        QTextCursor cursor(ui->markdownEditor->document()->findBlockByLineNumber(lineNo - 1));
        ui->markdownEditor->moveCursor(QTextCursor::End);
        ui->markdownEditor->setTextCursor(cursor);
        QString baseDocUrl = QUrl::fromLocalFile(fileInfo.canonicalFilePath()).toString();
        ui->markdownPreviewDoc->document()->setMetaInformation( QTextDocument::DocumentUrl,
                                                                baseDocUrl );
        ui->markdownPreviewDoc->document()->setBaseUrl(QUrl::fromLocalFile(fileInfo.canonicalFilePath()));
        setWindowTitle(QCoreApplication::translate("MainWindow", fileInfo.fileName().toStdString().c_str(), nullptr));
        SyncFolderSettings::setString(KEY_LAST_FILE, filePath);
        if (needSelect) {
            revealInTreeView();
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

        SyncFolderSettings::setString(KEY_LAST_FOLDER, folderPath);
    }

    QDirIterator it(currentRootDirPath, QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);

    fileNamesDictionary.clear();
    while (it.hasNext()) {
        QString dirName = it.next();
        std::string utf8_text = dirName.toUtf8().constData();
        fileNamesDictionary.push_back(utf8_text);
    }
}

void MainWindow::showInFolder(const QString& path)
{
    QFileInfo info(path);
#if defined(Q_OS_WIN)
    QStringList args;
    if (!info.isDir())
        args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args))
        return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + path + "\"";
    args << "-e";
    args << "end tell";
    args << "-e";
    args << "return";
    if (!QProcess::execute("/usr/bin/osascript", args))
        return;
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
}

/**
 * 左边文件浏览器的上下文菜单
 * @param pos
 */
void MainWindow::contextMenu(const QPoint &pos) {
    const QModelIndex curSelectedIndex = ui->fileTree->indexAt(pos);
    const QString currentSelectedFilePath = ui->fileTreeModel->filePath(curSelectedIndex);
    const QFileInfo currentSelectedFileInfo(currentSelectedFilePath);
    QMenu menu;
    QAction *removeAction = menu.addAction(tr("delete"));
    QAction *showInExplorerAction = menu.addAction(tr("show in explorer"));
#ifndef QT_NO_CLIPBOARD
    QAction *copyAction = menu.addAction(tr("copy path to clipboard"));
#endif
    QAction *setAsNewRoot = nullptr;
    QAction *createFolderUnderSelectedFolder = nullptr;
    QAction *addToFileQueue = nullptr;
    QAction *removeFromFileQueue = nullptr;
    QAction *moveAllQueuedFilesHere = nullptr;
    QAction *clearQueuedFiles = nullptr;
    QAction *createFolderUnderTopFolder = menu.addAction(tr("create folder under top folder"));
    bool hasAddFileQueueSeparator = false;
    if (currentSelectedFileInfo.isDir()) {
        setAsNewRoot = menu.addAction(tr("set as new root"));
        createFolderUnderSelectedFolder = menu.addAction(tr("create folder under selected folder"));
        if (!fileOperationQueue.isEmpty()) {
            if (!hasAddFileQueueSeparator) {
                menu.addSeparator();
                hasAddFileQueueSeparator = true;
            }
            moveAllQueuedFilesHere = menu.addAction(tr("move all queued files here"));
        }
    } else if (currentSelectedFileInfo.isFile()) {
        if (fileOperationQueue.contains(currentSelectedFilePath)) {
            if (!hasAddFileQueueSeparator) {
                menu.addSeparator();
                hasAddFileQueueSeparator = true;
            }
            removeFromFileQueue = menu.addAction(tr("remove from file queue"));
        } else {
            addToFileQueue = menu.addAction(tr("add to file queue"));
        }
    }
    if (!fileOperationQueue.isEmpty()) {
        if (!hasAddFileQueueSeparator) {
            menu.addSeparator();
            hasAddFileQueueSeparator = true;
        }
        clearQueuedFiles = menu.addAction(tr("clear all queued files"));
        DisplayQueuedFilesAction *displayQueuedFiles = new DisplayQueuedFilesAction(fileOperationQueue);
        menu.addAction(displayQueuedFiles);
    }
    QAction *action = menu.exec(ui->fileTree->mapToGlobal(pos));
    if (!action)
        return;
    if (action == removeAction) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Deletion confirm"), tr("Safe to delete?"),
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ui->fileTreeModel->remove(curSelectedIndex);
        }
    }
#ifndef QT_NO_CLIPBOARD
    else if (action == copyAction) {
        QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(currentSelectedFilePath));
    }
#endif
    else if (action == createFolderUnderTopFolder || action == createFolderUnderSelectedFolder) {
        auto index = (action == createFolderUnderTopFolder) ? ui->fileTreeModel->index(currentRootDirPath, 0) : curSelectedIndex;
        int i= 0;
        while (true) {
            QString name = (i == 0) ? QString(tr("untitled")) : QString(tr("untitled_%1")).arg(i);
            QFileInfo newFile = QFileInfo(currentRootDirPath, name);
            if (!newFile.exists()) {
                auto i = ui->fileTreeModel->mkdir(index, name);
                ui->fileTree->scrollTo(i);
                ui->fileTree->selectionModel()->select(i, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                break;
            }
            i++;
        }
    } else if (action == setAsNewRoot) {
        setCurrentRootDirPath(currentSelectedFilePath);
    } else if (action == addToFileQueue) {
        fileOperationQueue << currentSelectedFilePath;
    } else if (action == moveAllQueuedFilesHere && currentSelectedFileInfo.isDir()) {
        for (auto &filePath : fileOperationQueue) {
            QFileInfo qFileInfo(filePath);
            QFileInfo newFileInfo(currentSelectedFilePath, qFileInfo.fileName());
            QFile file(filePath);
            auto newFilePath = newFileInfo.absoluteFilePath();
            if (filePath != newFilePath) {
                file.rename(newFilePath);
            }
        }
        fileOperationQueue.clear();
    } else if (action == clearQueuedFiles) {
        fileOperationQueue.clear();
    } else if (action == removeFromFileQueue) {
        fileOperationQueue.remove(currentSelectedFilePath);
    } else if (action == showInExplorerAction) {
        showInFolder(currentSelectedFilePath);
    }
}

void MainWindow::processStdOutput()
{
//    qDebug()<< unisonProcess->readAllStandardOutput();
    syncLog += unisonProcess->readAllStandardOutput();  // read error channel
}

void MainWindow::processStdError()
{
//    qDebug()<< unisonProcess->readAllStandardError();
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
            msgBox.setText(tr("Error"));
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
            msgBox.setText(tr("Error"));
            msgBox.setInformativeText("file at path: " + dirPath + " is not able to be edited!");
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
       event->acceptProposedAction();
    }
    // if some actions should not be usable, like move, this code must be adopted
}

void MainWindow::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QMimeDatabase mimeDatabase;
        foreach (QUrl url, event->mimeData()->urls()) {
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
    newFileWithTitleContent(tr("untitled"), "");
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

void MainWindow::launchSettings() {
    SettingDialog dialog;
    dialog.exec();
}

void MainWindow::syncFiles() {
    syncLog.clear();

    if (syncProgressBar == nullptr) {
        syncProgressBar = new CircleProgressBar(tr("Sync is on-going"));
        statusBar()->addWidget(syncProgressBar);
    }

    syncProgressBar->setVisible(true);
    unisonProcess = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString syncConfigDirPath = getSyncConfigDir();
    env.insert("UNISON", syncConfigDirPath);
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

void MainWindow::setupMenus()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&Settings"), this, SLOT(launchSettings()),
                        QKeySequence(Qt::Key_F5));

    fileMenu->addSeparator();

    fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
                        QKeySequence::New);

    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()));

    fileMenu->addAction(tr("&Open dir..."), this, SLOT(openDirectory()));

    fileMenu->addAction(tr("&Save..."), this, SLOT(saveFile()),
                        QKeySequence::Save);

    fileMenu->addSeparator();

    fileMenu->addAction(tr("&Reveal in Tree View"), this, SLOT(revealInTreeView()),
                        QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_J));

    fileMenu->addAction(tr("&Sync"), this, SLOT(syncFiles()),
                        QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_S));

    fileMenu->addSeparator();

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

    // about menu
    QMenu *aboutMenu = new QMenu(tr("&about"), this);
    menuBar()->addMenu(aboutMenu);

    aboutMenu->addAction(tr("&about"), this, SLOT(about()),
                        QKeySequence(Qt::Key_Info));

    aboutMenu->addAction(tr("&help"), this, SLOT(help()),
                         QKeySequence(Qt::Key_Help));
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
        status = tr("successful synchronization; everything is up-to-date now.");
        emoji = "😀";
        break;
    case 1:
        status = tr("some files were skipped, but all file transfers were successful.");
        emoji = "😞";
        break;
    case 2:
        status = tr("non-fatal failures occurred during file transfer.");
        emoji = "😥";
        break;
    case 3:
        status = tr("a fatal error occurred, or the execution was interrupted.");
        emoji = "😭";
        break;
    default:
        status = tr("unknown");
        emoji = "💥";
        break;
    }

    QString result = QString(tr("%1 Sync result: %2, status: %3")).arg(emoji).arg(status).arg(exitCode);
    syncProgressBar->finish(result);
    syncProgressBar->setVisible(true);
}

void MainWindow::handleFileRenamed(const QString &path, const QString &oldName, const QString &newName) {
    setWindowTitle(QCoreApplication::translate("MainWindow", newName.toStdString().c_str(), nullptr));
    QFileInfo fileInfo(path, newName);
    openFile_l(fileInfo.filePath(), 1, true);
}

void MainWindow::revealInTreeView() {
    revealInTreeView_l(currentFilePath);
}

void MainWindow::about() {
    QMessageBox::about(this, tr("SyncFolder"),
              tr(
                 "<p><b>反馈</b>：<a href=\"http://syncfolder.chengxi.fun/\">官网</a>"
                 "<br /><p><b>版本</b>：" SYNC_FOLDER_VER
                 ));
}

void MainWindow::help() {
    QDesktopServices::openUrl(QUrl("https://syncfolder.chengxi.fun/help"));
}

void MainWindow::revealInTreeView_l(const QString &path) {
    if (!path.isEmpty()) {
        auto index = ui->fileTreeModel->index(path, 0);
        ui->fileTree->scrollTo(index);
        ui->fileTree->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void MainWindow::updateToc(const QVector<QStandardItem*> &nodes) {
    ui->tocModel->removeRows(0, ui->tocModel->rowCount());
    QStandardItem *rootNode = ui->tocModel->invisibleRootItem();
    for(auto &node : nodes) {
       rootNode->appendRow(node);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMarkdownPreview(const QStringList &images) {
    QString markdownText = ui->markdownEditor->toPlainText();
    ui->markdownPreviewDoc->preloadResources(images, markdownText);
    ui->markdownPreviewDoc->setMarkdown(markdownText);
}


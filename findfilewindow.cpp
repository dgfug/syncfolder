#include "findfilewindow.h"

#include <QCompleter>
#include <QGridLayout>
#include <QLabel>
#include <QShortcut>
#include <QTreeView>
#include <QHeaderView>
#include <QFile>
#include <QStandardItemModel>
#include <settings/settings_def.h>
#include <QDebug>
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

FindFileWindow::FindFileWindow(DMEditorDelegate *delegate, std::vector<std::string> &o_fileNamesDictionary, QWidget *parent)
    : QWidget(parent), fileNamesDictionary(o_fileNamesDictionary), editorDelegate(delegate)
{
    setWindowTitle(tr("文件查找"));
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("文件名:")), 0, 0);
    fileNameLineEdit = new QLineEdit(this);
    int width = DMSettings::getInt(KEY_LAST_WIN_WIDTH);
    width = width > 0 ? (width * 0.6f) : 300;
    fileNameLineEdit->setMinimumWidth(width);
    mainLayout->addWidget(fileNameLineEdit, 0, 1);
    fileNameLineEdit->setFocus();
    matchingFileNamesModel = new QStringListModel(this);
    fileNameCompleter = new QCompleter(matchingFileNamesModel, this);
    fileNameCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

    connect(fileNameLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(filenameSearchChanged(const QString &)));

    QTreeView *treeView = new QTreeView;
    fileNameCompleter->setPopup(treeView);
    fileNameCompleter->setMaxVisibleItems(1024);
    treeView->setRootIsDecorated(false);
    treeView->header()->hide();
    treeView->header()->setStretchLastSection(true);

    fileNameLineEdit->setCompleter(fileNameCompleter);

    connect(fileNameCompleter, QOverload<const QString &>::of(&QCompleter::activated),
        [=](const QString &text) {
        editorDelegate->openFile_l(text);
        this->hide();
        fileNameLineEdit->clear();
    });

    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
        this, &QWidget::hide);
}

void FindFileWindow::filenameSearchChanged(const QString &text)
{
    QStringList possibleNames;

        int score;
        std::vector<std::pair<int, std::string const*>> matches;
        for (auto && entry : fileNamesDictionary) {
            if (fts::fuzzy_match(text.toStdString().c_str(), entry.c_str(), score)) {
                matches.emplace_back(score, &entry);
            }
        }

        std::sort(matches.begin(), matches.end(), [](auto && a, auto && b) { return a.first > b.first; });

//        qDebug()<<"keyword: " << text;
        for (auto entry : matches) {
            possibleNames << QString(entry.second->c_str());
//            qDebug()<<"score: " << entry.first <<", path: " << entry.second->c_str();
        }
        matchingFileNamesModel->setStringList(possibleNames);
}

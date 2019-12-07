#include "FileLocatorWindow.h"

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
    connect(fileNameLineEdit, SIGNAL(returnPressed()), this, SLOT(openFirstFoundFile()));

    autoCompleteTreeView = new QTreeView;
    fileNameCompleter->setPopup(autoCompleteTreeView);
    fileNameCompleter->setMaxVisibleItems(1024);
    autoCompleteTreeView->setRootIsDecorated(false);
    autoCompleteTreeView->header()->hide();
    autoCompleteTreeView->header()->setStretchLastSection(true);

    fileNameLineEdit->setCompleter(fileNameCompleter);

    connect(fileNameCompleter, QOverload<const QString &>::of(&QCompleter::activated),
        [=](const QString &text) {
        editorDelegate->openFile_l(text, 1);
        this->hide();
        fileNameLineEdit->clear();
    });

    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
        this, &QWidget::hide);

    QFont font = QFont();
    font.setPointSize(16);
//    font.setFamily("Source Code Variable");
    this->setFont(font);
    this->setStyleSheet("QWidget {background-color:#FFFAE4; color:#434C5B; selection-background-color:#B5D788;}");
}

void FindFileWindow::openFirstFoundFile() {
    const QString path = fileNameCompleter->completionModel()->data(
                fileNameCompleter->completionModel()->index(0,0)).toString();
    editorDelegate->openFile_l(path, 1, true);
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

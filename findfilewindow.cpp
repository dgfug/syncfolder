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

QAbstractItemModel *FindFileWindow::createModel() {
//    int score;
//    std::vector<std::pair<int, std::string const*>> matches;
//    for (auto && entry : fileNamesDictionary) {
//        if (fts::fuzzy_match(keyword.toStdString().c_str(), entry.c_str(), score)) {
//            matches.emplace_back(score, &entry);
//        }
//    }

//    std::sort(matches.begin(), matches.end(), [](auto && a, auto && b) { return a.first > b.first; });

    std::size_t matchCount = fileNamesDictionary.size();
    QStandardItemModel *m = new QStandardItemModel(matchCount, 1, fileNameCompleter);

    for (std::size_t i = 0; i < matchCount; i++) {
        auto entry = fileNamesDictionary[i];
        QModelIndex fileNameIdx = m->index(i, 0);
        m->setData(fileNameIdx, QString(entry.c_str()));
    }

    return m;
}

FindFileWindow::FindFileWindow(std::vector<std::string> &o_fileNamesDictionary, QWidget *parent)
    : QWidget(parent), fileNamesDictionary(o_fileNamesDictionary)
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
    fileNameCompleter = new QCompleter(this);
    fileNameCompleter->setCompletionMode(QCompleter::PopupCompletion);
    fileNameCompleter->setFilterMode(Qt::MatchContains);

    fileNameCompleter->setModel(createModel());
    QTreeView *treeView = new QTreeView;
    fileNameCompleter->setPopup(treeView);
    treeView->setRootIsDecorated(true);
    treeView->header()->hide();

    fileNameLineEdit->setCompleter(fileNameCompleter);

    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
        this, &QWidget::hide);
}

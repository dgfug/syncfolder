/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QJsonDocument>
#include <QJsonObject>
#include "FullTextSearchWindow.h"

#include "settings/settings_def.h"
#include "fileformat.h"

enum { filePathRole = Qt::UserRole + 1 };
enum { lineNoRole = Qt::UserRole + 2 };
enum { contentRole = Qt::UserRole + 3 };
enum { itemTypeRole = Qt::UserRole + 4 };

FullTextSearchWindow::FullTextSearchWindow(DMEditorDelegate *delegate, const QString docDir, QWidget *parent)
    : QWidget(parent), editorDelegate(delegate), rgTaskProcess(nullptr), currentDir(docDir), copyButton(nullptr), findButton(
        nullptr)
{
    setWindowTitle(tr("Find content search with keyword"));
    textComboBox = createComboBox();
    connect(textComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &FullTextSearchWindow::animateFindClick);

    filesFoundLabel = new QLabel;

    createFoundFilesTree();

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Containing text:")), 1, 0);
    mainLayout->addWidget(textComboBox, 1, 1, 1, 2);
    mainLayout->addWidget(new QLabel(tr("In directory:")), 2, 0);
    mainLayout->addWidget(foundFilesTree, 2, 0, 1, 3);
    mainLayout->addWidget(filesFoundLabel, 3, 0, 1, 2);

    copyButton = new QPushButton(tr("&Copy"), this);
    connect(copyButton, &QAbstractButton::clicked, this, &FullTextSearchWindow::copy);

    findButton = new QPushButton(tr("&Find"), this);
    connect(findButton, &QAbstractButton::clicked, this, &FullTextSearchWindow::find);

    mainLayout->addWidget(copyButton, 3, 2);
    mainLayout->addWidget(findButton, 3, 3);

    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated,
        this, &QWidget::hide);

    textComboBox->setFocus();
}

static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

//! [3]
void FullTextSearchWindow::find()
{
    QString text = textComboBox->currentText();
    updateComboBox(textComboBox);

    rgTaskStdout.clear();
    rgTaskStderror.clear();
    FullTextSearchWindow *that = this;
    rgTaskProcess = new QProcess(this);
    connect(rgTaskProcess, SIGNAL(finished(int, QProcess::ExitStatus)), that, SLOT(handleRgTaskFinished(int, QProcess::ExitStatus)));
    connect(rgTaskProcess, SIGNAL(readyReadStandardOutput()), that, SLOT(processRgStdOutput()));
    connect(rgTaskProcess, SIGNAL(readyReadStandardError()), that, SLOT(processRgStdError()));
    QString command("rg");
    QStringList arguments;
    arguments <<"--json" << text << currentDir;
    rgTaskProcess->start(command, arguments);
}
//! [4]

void FullTextSearchWindow::animateFindClick()
{
    findButton->animateClick();
}

void FullTextSearchWindow::showFiles(const QString &results)
{
    QStringList rgResult = rgTaskStdout.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);

    QVector<QStandardItem*> items;
    QStandardItem *fileBeginItem;
    foundFilesModel->removeRows(0, foundFilesModel->rowCount());
    QStandardItem *rootNode = foundFilesModel->invisibleRootItem();

    for (auto &r : rgResult) {
        auto json_doc= QJsonDocument::fromJson(r.toUtf8());
        if (json_doc.isObject()) {
            QJsonObject jsonObj = json_doc.object();
            if (jsonObj.contains("type")) {
                if (jsonObj.value("type") == "begin") {
                    if (jsonObj.contains("data") && jsonObj.value("data").toObject().contains("path")) {
                        QString path = jsonObj.value("data").toObject().value("path").toObject().value("text").toString();
                        fileBeginItem = new QStandardItem(path);
                        fileBeginItem->setData((QString)path, filePathRole);
                        fileBeginItem->setData(QString("begin"), itemTypeRole);
                        fileBeginItem->setEditable(false);
                        rootNode->appendRow(fileBeginItem);
                    }
                } else if (jsonObj.value("type") == "match") {
                    if (jsonObj.contains("data") && jsonObj.value("data").toObject().contains("path")) {
                        QJsonObject data = jsonObj.value("data").toObject();
                        QString path = data.value("path").toObject().value("text").toString();
                        QString lines = data.value("lines").toObject().value("text").toString().trimmed();
                        int line_number = data.value("line_number").toInt();
                        QStandardItem *fileMatchItem = new QStandardItem(lines);
                        fileMatchItem->setData((QString)path, filePathRole);
                        fileMatchItem->setData((qlonglong)line_number, lineNoRole);
                        fileMatchItem->setData(lines, contentRole);
                        fileMatchItem->setData(QString("match"), itemTypeRole);
                        fileMatchItem->setEditable(false);

                        if (fileBeginItem) {
                            fileBeginItem->appendRow(fileMatchItem);
                        }
                    }
                } else if (jsonObj.value("type") == "end") {
                    if (jsonObj.contains("data") && jsonObj.value("data").toObject().contains("stats")) {
                        int matches = jsonObj.value("data").toObject().value("stats").toObject().value("matches").toInt();
                        fileBeginItem->setText(fileBeginItem->text() + tr("(%n matches)", nullptr, matches));
                    }
                    fileBeginItem = nullptr;
                } else if (jsonObj.value("type") == "summary") {
                    if (jsonObj.contains("data") && jsonObj.value("data").toObject().contains("stats")) {
                        QJsonObject stats = jsonObj.value("data").toObject().value("stats").toObject();
                        if (stats.contains("elapsed")) {
                            QString elapsed = stats.value("elapsed").toObject().value("human").toString();
                            if (stats.contains("matches")) {
                                int matches = stats.value("matches").toInt();
                                filesFoundLabel->setText(tr("%n matches found, consumes ", nullptr, matches) + elapsed);
                                filesFoundLabel->setWordWrap(true);
                            }
                        }
                    }
                }
            }
        }
    }

    foundFilesTree->expandAll();
}
//! [8]

//! [10]
QComboBox *FullTextSearchWindow::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}
//! [10]

//! [11]
void FullTextSearchWindow::createFoundFilesTree()
{
    foundFilesTree = new QTreeView;
    foundFilesModel = new QStandardItemModel(this);
    foundFilesTree->setModel(foundFilesModel);
    foundFilesTree->expandAll();
    foundFilesTree->setExpandsOnDoubleClick(false);
    QObject::connect(foundFilesTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(openFileOfItem(const QItemSelection&,const QItemSelection&)));
}

void FullTextSearchWindow::openFileOfItem(const QItemSelection& selected, const QItemSelection&)
{
    QModelIndexList selectedList = selected.indexes();
    if (!selectedList.empty()) {
        auto curSelectedIndex = selectedList[0];

        QString filePath = foundFilesModel->data(curSelectedIndex, filePathRole).toString();
        qlonglong lineNo = foundFilesModel->data(curSelectedIndex, lineNoRole).toLongLong();
        QString matchedContent = foundFilesModel->data(curSelectedIndex, contentRole).toString();
        QString itemType = foundFilesModel->data(curSelectedIndex, itemTypeRole).toString();

        if (lineNo > 0) {
            editorDelegate->openFile_l(filePath, lineNo, true);
        }
    }
}

void FullTextSearchWindow::copy() {
    auto curSelectedIndex = foundFilesTree->selectionModel()->currentIndex();

    QString filePath = foundFilesModel->data(curSelectedIndex, filePathRole).toString();
    QString matchedContent = foundFilesModel->data(curSelectedIndex, contentRole).toString();
    QString itemType = foundFilesModel->data(curSelectedIndex, itemTypeRole).toString();

    if (itemType == "match") {
        QGuiApplication::clipboard()->setText(matchedContent);
    } else if (itemType == "begin") {
        QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(filePath));
    }
}

void FullTextSearchWindow::handleRgTaskFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == EXIT_SUCCESS) {
        showFiles(rgTaskStdout);
        rgTaskStdout.clear();
    }
}

void FullTextSearchWindow::processRgStdOutput() {
    rgTaskStdout += rgTaskProcess->readAllStandardOutput();
}

void FullTextSearchWindow::processRgStdError() {
    rgTaskStderror += rgTaskProcess->readAllStandardError();
}
//! [16]

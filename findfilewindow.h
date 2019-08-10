#ifndef FINDFILEWINDOW_H
#define FINDFILEWINDOW_H

#include <QAbstractItemModel>
#include <QLineEdit>
#include <QStringListModel>
#include <QWidget>
#include "editerdelegate.h"

class FindFileWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FindFileWindow(DMEditorDelegate *editorDelegate, std::vector<std::string> &fileNamesDictionary, QWidget *parent = nullptr);

signals:

public slots:
    void filenameSearchChanged(const QString &text);

private:
    QLineEdit *fileNameLineEdit;
    QCompleter *fileNameCompleter;
    std::vector<std::string> &fileNamesDictionary;
    QStringListModel *matchingFileNamesModel;
    DMEditorDelegate *editorDelegate;
};

#endif // FINDFILEWINDOW_H

#ifndef FINDFILEWINDOW_H
#define FINDFILEWINDOW_H

#include <QAbstractItemModel>
#include <QLineEdit>
#include <QWidget>

class FindFileWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FindFileWindow(std::vector<std::string> &fileNamesDictionary, QWidget *parent = nullptr);

signals:

public slots:
    QAbstractItemModel *createModel();

private:
    QLineEdit *fileNameLineEdit;
    QCompleter *fileNameCompleter;
    std::vector<std::string> &fileNamesDictionary;
};

#endif // FINDFILEWINDOW_H

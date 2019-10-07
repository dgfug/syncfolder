#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>

class SettingDialog : public QDialog
{
    Q_OBJECT
public:
    SettingDialog();

signals:

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void createIcons();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
};

#endif // SETTINGDIALOG_H

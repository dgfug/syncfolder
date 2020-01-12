#include "DisplayQueuedFilesAction.h"

#include <QBoxLayout>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
DisplayQueuedFilesAction::DisplayQueuedFilesAction (const QSet<QString> &queuedFiles) :
        QWidgetAction (NULL) {
    QWidget* pWidget = new QWidget (NULL);
    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    QLabel *label = new QLabel(tr("queued files"));
    vBoxLayout->addWidget(label);
    QListWidget* pListWidget = new QListWidget(pWidget);
    int row = 0;
    for (auto &file: queuedFiles) {
        QFileInfo fileInfo(file);
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(fileInfo.fileName());
        pListWidget->insertItem(row++, newItem);
    }
    vBoxLayout->addWidget (pListWidget);
    pWidget->setLayout (vBoxLayout);
    setDefaultWidget(pWidget);
}

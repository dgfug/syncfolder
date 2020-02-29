#ifndef EDITERDELEGATE_H
#define EDITERDELEGATE_H

#include <QString>
#include <QStandardItem>

class DMEditorDelegate {
public:
    virtual void openFile_l(const QString &, size_t lineNo, bool needSelect = false) = 0;
    virtual void updateToc(const QVector<QStandardItem*> &nodes) = 0;
};

#endif // EDITERDELEGATE_H

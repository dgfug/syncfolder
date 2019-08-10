#ifndef EDITERDELEGATE_H
#define EDITERDELEGATE_H

#include <QString>

class DMEditorDelegate {
public:
    virtual void openFile_l(const QString &) = 0;
};

#endif // EDITERDELEGATE_H

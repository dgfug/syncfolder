#ifndef DMFILEICONPROVIDER_H
#define DMFILEICONPROVIDER_H

#include <QFileIconProvider>

class DMFileIconProvider : public QFileIconProvider
{
public:
    explicit DMFileIconProvider();
//    virtual QIcon icon(IconType type) const override;
    virtual QIcon icon(const QFileInfo &info) const override;
//    virtual QString type(const QFileInfo &info) const override;
};

#endif // DMFILEICONPROVIDER_H

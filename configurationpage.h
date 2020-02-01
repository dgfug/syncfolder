#ifndef CONFIGURATIONPAGE_H
#define CONFIGURATIONPAGE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>
#include <ISettingPage.h>

class ConfigurationPage : public QWidget, ISettingPage
{
public:
    ConfigurationPage(QWidget *parent = 0);
    void save() override;
    void reset() override;

private:
    QLineEdit *serviceApiEndPointEdit;
    QLineEdit *passwordEdit;
    QLineEdit *usernameEdit;
};

#endif // CONFIGURATIONPAGE_H

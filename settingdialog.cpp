#include "settingdialog.h"
#include "configurationpage.h"

#include <QBoxLayout>
#include <QPushButton>

SettingDialog::SettingDialog()
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(64, 46));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(88);
    contentsWidget->setSpacing(12);

    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(new ConfigurationPage);
//    pagesWidget->addWidget(new ConfigurationPage);

    QPushButton *saveButton = new QPushButton(tr("Save"));

    createIcons();
    contentsWidget->setCurrentRow(0);

    connect(saveButton, &QAbstractButton::clicked, this, &SettingDialog::save);

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget, 0);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(saveButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Setting Dialog"));
    setMinimumWidth(500);
}

void SettingDialog::createIcons()
{
    QListWidgetItem *configButton = new QListWidgetItem(contentsWidget);
    configButton->setIcon(QIcon(":/icons/update.png"));
    configButton->setText(tr("Sync"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(contentsWidget, &QListWidget::currentItemChanged, this, &SettingDialog::changePage);
}

void SettingDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void SettingDialog::save() {
    dynamic_cast<ConfigurationPage*>(pagesWidget->currentWidget())->save();
    close();
}

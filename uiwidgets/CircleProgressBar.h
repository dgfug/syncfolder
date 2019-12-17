#pragma once

#include "ProgressCircle.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QMovie>

class CircleProgressBar : public QWidget
{

public:
    explicit CircleProgressBar(QString text, QWidget * parent = 0) :
        QWidget(parent)
    {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setSpacing(12);
        syncAnimation = new QMovie(":/animations/sync.apng", "apng");
        syncAnimation->setScaledSize(QSize(24, 24));
        syncAnimation->start();
        label = new QLabel(text);
        label->setMovie(syncAnimation);
        layout->addWidget(label, 0);
        QPushButton * closeButton = new QPushButton(tr("Got it"));
        connect(closeButton, &QPushButton::clicked, this, [&]() {
            this->setVisible(false);
        });
        layout->addWidget(closeButton);
    }

    void finish(QString result)
    {
        label->setText(result);
    }

private:
    QLabel *label;
    QMovie *syncAnimation;
};

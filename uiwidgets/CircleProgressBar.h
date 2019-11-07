#pragma once

#include "ProgressCircle.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

class CircleProgressBar : public QWidget
{

public:
    explicit CircleProgressBar(QString text, QWidget * parent = 0) :
        QWidget(parent),
        progressCircleAnimation(nullptr)
    {
        setStyleSheet(".QWidget{border-bottom: 1px solid lightgrey; padding: 0px;}");
        QHBoxLayout * layout = new QHBoxLayout(this);
        layout->setSpacing(12);
        progressCircle = new ProgressCircle();
        progressCircle->setFixedSize(30, 30);
        layout->addWidget(progressCircle);
        label = new QLabel(text);
        layout->addWidget(label, 0);
        QPushButton * closeButton = new QPushButton(tr("close"));
        connect(closeButton, &QPushButton::clicked, this, &QObject::deleteLater);
        layout->addWidget(closeButton);

        if (progressCircleAnimation == nullptr) {
            progressCircleAnimation = new QPropertyAnimation(progressCircle, "outerRadius", progressCircle);
            progressCircleAnimation->setDuration(750);
            progressCircleAnimation->setEasingCurve(QEasingCurve::OutQuad);
            progressCircleAnimation->setStartValue(1.0);
            progressCircleAnimation->setEndValue(3.8);
            progressCircleAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }

    void finish(QString result)
    {
//        QPropertyAnimation * animation = new QPropertyAnimation(progressCircle, "outerRadius", progressCircle);
//        animation->setDuration(1500);
//        animation->setEasingCurve(QEasingCurve::OutQuad);
//        animation->setEndValue(0.5);
//        animation->start(QAbstractAnimation::KeepWhenStopped);

//        animation = new QPropertyAnimation(progressCircle, "innerRadius", progressCircle);
//        animation->setDuration(750);
//        animation->setEasingCurve(QEasingCurve::OutQuad);
//        animation->setEndValue(0.0);
//        animation->start(QAbstractAnimation::KeepWhenStopped);

//        QColor color = success ? QColor(155,219,58) : QColor(255,100,100);

//        animation = new QPropertyAnimation(progressCircle, "color", progressCircle);
//        animation->setDuration(750);
//        animation->setEasingCurve(QEasingCurve::OutQuad);
//        animation->setEndValue(color);
//        animation->start(QAbstractAnimation::KeepWhenStopped);

        label->setText(result);
//        if (progressCircleAnimation != nullptr) {
//            progressCircleAnimation->stop();
//        }
    }

private:
    QLabel *label;
    ProgressCircle *progressCircle;
    QPropertyAnimation *progressCircleAnimation;
};

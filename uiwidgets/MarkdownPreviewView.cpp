//
// Created by faywong on 2020/1/28.
//

#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "MarkdownPreviewView.h"

QPixmap MarkdownPreviewView::downloadOnlineImg(const QString& url){
    QNetworkAccessManager nam;
    QEventLoop loop;
    QObject::connect(&nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QNetworkReply *reply = nam.get(QNetworkRequest(url));
    loop.exec();
    QPixmap pm;
    pm.loadFromData(reply->readAll());
    delete reply;
    return pm;
}

QVariant MarkdownPreviewView::loadResource(int type, const QUrl &name) {
    if (type==QTextDocument::ImageResource
            && (name.scheme() == "http" || name.scheme() == "https")) {
        //        qDebug()<<"load res: " << name;
        QImage image(64, 64, QImage::Format_RGB32);
        image.fill(qRgb(255, 160, 128));
        return QVariant(image);
//        return QVariant(this->downloadOnlineImg(name.toString()));
    } else {
        return QTextBrowser::loadResource(type, name);
    }
}

MarkdownPreviewView::MarkdownPreviewView(QWidget *parent) : QTextBrowser(parent) {

}

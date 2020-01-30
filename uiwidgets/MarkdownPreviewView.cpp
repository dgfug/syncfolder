//
// Created by faywong on 2020/1/28.
//

#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QDesktopServices>
#include "MarkdownPreviewView.h"


MarkdownPreviewView::MarkdownPreviewView(QWidget *parent) : QTextBrowser(parent), resourceCache(/* maxCost */30), nam(new QNetworkAccessManager(this)) {
    connect(nam, &QNetworkAccessManager::finished, this, &MarkdownPreviewView::downloadFinished);
}

bool MarkdownPreviewView::preloadResources(const QStringList &resList) {
    QSet<QString> resSet(resList.begin(), resList.end());
    bool needWaitResReady = false;
    for (auto image : resSet) {
        const QUrl url = QUrl(image);
        if ((url.scheme() == "http" || url.scheme() == "https")
                && !resourceCache.contains(image)) {
            needWaitResReady = true;
//            qDebug()<<"preloadResources url: " << url.toString();
            QNetworkRequest request(url);
            nam->get(request);
        }
    }
    return needWaitResReady;
}

void MarkdownPreviewView::downloadFinished(QNetworkReply *reply) {
    QPixmap *pm = new QPixmap();
    pm->loadFromData(reply->readAll());
    resourceCache.insert(reply->url().toString(), pm);
//    qDebug()<<"downloadFinished url: " << reply->url().toString() << " => pm: " << pm;
    viewport()->update();
}

QVariant MarkdownPreviewView::loadResource(int type, const QUrl &name) {
    if (type==QTextDocument::ImageResource
            && (name.scheme() == "http" || name.scheme() == "https")) {
//                qDebug()<<"loadResource res: " << name;
        if (resourceCache.contains(name.toString())) {
            return *resourceCache.take(name.toString());
        }
    }
    return QTextBrowser::loadResource(type, name);
}

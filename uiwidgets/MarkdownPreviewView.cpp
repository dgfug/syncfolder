//
// Created by faywong on 2020/1/28.
//

#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QDesktopServices>
#include <QPainter>
#include "MarkdownPreviewView.h"

MarkdownPreviewView::MarkdownPreviewView(QWidget *parent) : QTextBrowser(parent), resourceCache(/* maxCost */30), nam(new QNetworkAccessManager(this)) {
    connect(nam, &QNetworkAccessManager::finished, this, &MarkdownPreviewView::downloadFinished);
    setOpenLinks(false);
}

bool MarkdownPreviewView::preloadResources(const QStringList &resList, const QString markdownText) {
    bool needWaitResReady = false;
    QSet<QString> resSet(resList.begin(), resList.end());
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
    if (needWaitResReady) {
        pendingMarkdownText = markdownText;
    }
    return needWaitResReady;
}

void MarkdownPreviewView::downloadFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QPixmap *pm = new QPixmap();
        pm->loadFromData(reply->readAll());
        QString url = reply->url().toString();
        resourceCache.insert(url, pm);
        if (!pendingMarkdownText.isEmpty()) {
//            qDebug()<<"relayout after downloadFinished res: " << url;
            setMarkdown(pendingMarkdownText);
        }
    }
}

QVariant MarkdownPreviewView::loadResource(int type, const QUrl &name) {
    if (type==QTextDocument::ImageResource
            && (name.scheme() == "http" || name.scheme() == "https")) {
        if (resourceCache.contains(name.toString())) {
            qDebug()<<"loadResource found res: " << name;
//            QImage image(QSize(400,300),QImage::Format_RGB32);
//            QPainter painter(&image);
//            painter.setBrush(QBrush(Qt::green));
//            painter.fillRect(QRectF(0,0,400,300),Qt::green);
//            painter.fillRect(QRectF(100,100,200,100),Qt::white);
//            painter.setPen(QPen(Qt::black));
//            painter.drawText(QRect(100,100,200,100),"Text you want to draw...");
//            return QVariant(image);
            return *resourceCache.take(name.toString());
        }
    }
    return QTextBrowser::loadResource(type, name);
}

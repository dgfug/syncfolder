//
// Created by faywong on 2020/1/28.
//

#ifndef SYNCFOLDER_MARKDOWNPREVIEWVIEW_H
#define SYNCFOLDER_MARKDOWNPREVIEWVIEW_H

#include <QtWidgets/QTextBrowser>
#include <QtCore/QCache>
#include <QNetworkReply>
class MarkdownPreviewView : public QTextBrowser {
public:
    explicit MarkdownPreviewView(QWidget* parent = nullptr);
    bool preloadResources(const QStringList &images);

protected:
    QVariant loadResource(int type, const QUrl &name) override;

protected slots:
    void downloadFinished(QNetworkReply *reply);

private:
    QCache<QString, QPixmap> resourceCache;
    QNetworkAccessManager *nam;
};

#endif //SYNCFOLDER_MARKDOWNPREVIEWVIEW_H

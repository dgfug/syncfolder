//
// Created by faywong on 2020/1/28.
//

#ifndef SYNCFOLDER_MARKDOWNPREVIEWVIEW_H
#define SYNCFOLDER_MARKDOWNPREVIEWVIEW_H


#include <QtWidgets/QTextBrowser>

class MarkdownPreviewView : public QTextBrowser {
public:
    explicit MarkdownPreviewView(QWidget* parent = nullptr);

protected:
    QVariant loadResource(int type, const QUrl &name) override;

    QPixmap downloadOnlineImg(const QString &url);
};


#endif //SYNCFOLDER_MARKDOWNPREVIEWVIEW_H

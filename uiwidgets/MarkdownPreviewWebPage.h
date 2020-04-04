//
// Created by faywong on 2020/4/4.
//

#ifndef SYNCFOLDER_MARKDOWNPREVIEWWEBPAGE_H
#define SYNCFOLDER_MARKDOWNPREVIEWWEBPAGE_H

#include <QWebEnginePage>

class MarkdownPreviewWebPage : public QWebEnginePage {
public:
    MarkdownPreviewWebPage(QWebEngineProfile *profile, QObject *parent);

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
};


#endif //SYNCFOLDER_MARKDOWNPREVIEWWEBPAGE_H

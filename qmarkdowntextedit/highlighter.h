/* PEG Markdown Highlight
 * Copyright 2011-2016 Ali Rantakari -- http://hasseg.org
 * Licensed under the GPL2+ and MIT licenses (see LICENSE for more info).
 * 
 * highlighter.h
 * 
 * Qt 4.7 example for highlighting a rich text widget.
 */

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QTextCharFormat>
#include <QThread>
#include <QFuture>
#include "EditorDelegate.h"

extern "C" {
#include "pmh_parser.h"
}

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

struct HighlightingStyle
{
    pmh_element_type type;
    QTextCharFormat format;
};


class HGMarkdownHighlighter : public QObject
{
    Q_OBJECT

public:
    HGMarkdownHighlighter(QTextDocument *parent, DMEditorDelegate *mainWindow, int aWaitInterval = 250);
    void setStyles(QVector<HighlightingStyle> &styles);
    void highlight(pmh_element **result);

signals:
    void parseFinished(pmh_element **result);
    void md2htmlFinished(const QString &html);

private slots:
    void handleContentsChange(int position, int charsRemoved, int charsAdded);
    void timerTimeout();

private:
    DMEditorDelegate *mainWin;
    QTextDocument *document;
    QFuture<void> parseTaskFuture;
    QFuture<void> toMdTaskFuture;
    QVector<HighlightingStyle> *highlightingStyles;
    int waitInterval;
    QTimer *timer;

    void clearFormatting();
    void parse();
    void setDefaultStyles();
};

#endif

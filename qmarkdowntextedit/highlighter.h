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
    HGMarkdownHighlighter(QTextDocument *parent = nullptr, int aWaitInterval = 10);
    void setStyles(QVector<HighlightingStyle> &styles);
    int waitInterval;

protected:

private slots:
    void handleContentsChange(int position, int charsRemoved, int charsAdded);
    void timerTimeout();

private:
    QTimer *timer;
    QTextDocument *document;
    QFuture<void> parseTaskFuture;
    QVector<HighlightingStyle> *highlightingStyles;

    void clearFormatting();
    void highlight(pmh_element **result);
    void parse();
    void setDefaultStyles();
};

#endif

/* PEG Markdown Highlight
 * Copyright 2011-2016 Ali Rantakari -- http://hasseg.org
 * Licensed under the GPL2+ and MIT licenses (see LICENSE for more info).
 * 
 * highlighter.cpp
 * 
 * Qt 4.7 example for highlighting a rich text widget.
 */

#include <QtGui>
#include "highlighter.h"


WorkerThread::~WorkerThread()
{
    if (result != NULL)
        pmh_free_elements(result);
    free(content);
}
void WorkerThread::run()
{
    if (content == nullptr)
        return;

    pmh_markdown_to_elements(content, pmh_EXT_NOTES | pmh_EXT_STRIKE, &result);
}

HGMarkdownHighlighter::HGMarkdownHighlighter(QTextDocument *parent,
                                             int aWaitInterval) : QObject(parent)
{
    highlightingStyles = NULL;
    workerThread = NULL;
    cached_elements = NULL;
    waitInterval = aWaitInterval;
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(aWaitInterval);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    document = parent;
    connect(document, SIGNAL(contentsChange(int,int,int)),
            this, SLOT(handleContentsChange(int,int,int)));

    this->parse();
}

void HGMarkdownHighlighter::setStyles(QVector<HighlightingStyle> &styles)
{
    this->highlightingStyles = &styles;
}

#define STY(type, format) styles->append((HighlightingStyle){type, format})
void HGMarkdownHighlighter::setDefaultStyles()
{
    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    // TODO: support theme
    // base16-atelierseaside.light theme
    int defaultFontSize = 14;
    QTextCharFormat headers;
    headers.setForeground(QBrush(QColor("#242A2D")));
    // 遵循 css 默认 size
    headers.setFontWeight(QFont::Black);
    headers.setFontPointSize(defaultFontSize * 2.0);
    STY(pmh_H1, headers);
    headers.setFontWeight(QFont::ExtraBold);
    headers.setFontPointSize(defaultFontSize * 1.8);
    STY(pmh_H2, headers);
    headers.setFontWeight(QFont::Bold);
    headers.setFontPointSize(defaultFontSize * 1.6);
    STY(pmh_H3, headers);
    headers.setFontWeight(QFont::DemiBold);
    headers.setFontPointSize(defaultFontSize * 1.4);
    STY(pmh_H4, headers);
    headers.setFontWeight(QFont::Medium);
    headers.setFontPointSize(defaultFontSize * 1.3);
    STY(pmh_H5, headers);
    headers.setFontWeight(QFont::Normal);
    headers.setFontPointSize(defaultFontSize * 1.2);
    STY(pmh_H6, headers);

    QTextCharFormat hrule;
    hrule.setForeground(QBrush(QColor("#5E6E5E")));
    STY(pmh_HRULE, hrule);

    QTextCharFormat list;
    list.setForeground(QBrush(QColor("#E6193C")));
    STY(pmh_LIST_BULLET, list);
    STY(pmh_LIST_ENUMERATOR, list);

    QTextCharFormat link;
    link.setForeground(QBrush(QColor("#3D62F5")));
    STY(pmh_LINK, link);
    STY(pmh_AUTO_LINK_URL, link);
    STY(pmh_AUTO_LINK_EMAIL, link);

    QTextCharFormat image;
    image.setForeground(QBrush(QColor("#E6193C")));
    STY(pmh_IMAGE, image);

    QTextCharFormat ref;
    ref.setForeground(QBrush(QColor("#87711D")));
    STY(pmh_REFERENCE, ref);

    QTextCharFormat code;
    code.setBackground(QBrush(QColor("#66AAAAAA")));
    // `abc` ```javascript ... 这种
    STY(pmh_CODE, code);
    STY(pmh_VERBATIM, code);

    /* Emphasis, aka italics, with *asterisks* or _underscores_. */
    QTextCharFormat emph;
    emph.setForeground(QBrush(QColor("#AD2BEE")));
    emph.setFontItalic(true);
    STY(pmh_EMPH, emph);

    /* Strong emphasis, aka bold, with **asterisks** or __underscores__. */
    QTextCharFormat strong;
    strong.setForeground(QBrush(QColor("#98981B")));
    strong.setFontWeight(QFont::DemiBold);
    STY(pmh_STRONG, strong);

    QTextCharFormat comment; comment.setForeground(QBrush(QColor("#809980")));
    STY(pmh_COMMENT, comment);

    QTextCharFormat blockquote;
    blockquote.setForeground(QBrush(QColor("#87711D")));
    blockquote.setBackground(QBrush(QColor("#F0F0F0")));
    STY(pmh_BLOCKQUOTE, blockquote);

    QTextCharFormat note;
    note.setForeground(QBrush(QColor("#AD2BEE")));
    note.setBackground(QBrush(QColor("#F0F0F0")));

    STY(pmh_NOTE, note);

    QTextCharFormat strike;
    strike.setForeground(QBrush(QColor("#D55B7B")));
    strike.setFontStrikeOut(true);
    STY(pmh_STRIKE, strike);

    QTextCharFormat htmlBlock;
    htmlBlock.setForeground(QBrush(QColor("#E6193C")));
    htmlBlock.setBackground(QBrush(QColor("#CFE8CF")));

    STY(pmh_HTMLBLOCK, htmlBlock);
    this->setStyles(*styles);
}

void HGMarkdownHighlighter::clearFormatting()
{
    QTextBlock block = document->firstBlock();
    while (block.isValid()) {
        block.layout()->clearAdditionalFormats();
        block = block.next();
    }
}

void HGMarkdownHighlighter::highlight()
{
    if (cached_elements == NULL) {
        qDebug() << "cached_elements is NULL";
        return;
    }

    if (highlightingStyles == NULL)
        this->setDefaultStyles();

    this->clearFormatting();

    for (int i = 0; i < highlightingStyles->size(); i++)
    {
        HighlightingStyle style = highlightingStyles->at(i);
        pmh_element *elem_cursor = cached_elements[style.type];
        while (elem_cursor != NULL)
        {
            if (elem_cursor->end <= elem_cursor->pos) {
                elem_cursor = elem_cursor->next;
                continue;
            }

            // "The QTextLayout object can only be modified from the
            // documentChanged implementation of a QAbstractTextDocumentLayout
            // subclass. Any changes applied from the outside cause undefined
            // behavior." -- we are breaking this rule here. There might be
            // a better (more correct) way to do this.

            int startBlockNum = document->findBlock(elem_cursor->pos).blockNumber();
            int endBlockNum = document->findBlock(elem_cursor->end).blockNumber();
            for (int j = startBlockNum; j <= endBlockNum; j++)
            {
                QTextBlock block = document->findBlockByNumber(j);

                QTextLayout *layout = block.layout();
                QList<QTextLayout::FormatRange> list = layout->additionalFormats();
                int blockpos = block.position();
                QTextLayout::FormatRange r;
                r.format = style.format;

                if (j == startBlockNum) {
                    r.start = elem_cursor->pos - blockpos;
                    r.length = (startBlockNum == endBlockNum)
                                ? elem_cursor->end - elem_cursor->pos
                                : block.length() - r.start;
                } else if (j == endBlockNum) {
                    r.start = 0;
                    r.length = elem_cursor->end - blockpos;
                } else {
                    r.start = 0;
                    r.length = block.length();
                }

                list.append(r);
                layout->setAdditionalFormats(list);
            }

            elem_cursor = elem_cursor->next;
        }
    }

    document->markContentsDirty(0, document->characterCount());
}

void HGMarkdownHighlighter::parse()
{
    if (workerThread != NULL && workerThread->isRunning()) {
        parsePending = true;
        return;
    }

    QString content = document->toPlainText();
    QByteArray ba = content.toUtf8();
    char *content_cstring = strdup((char *)ba.data());

    if (workerThread != nullptr)
        delete workerThread;
    workerThread = new WorkerThread();

    workerThread->content = content_cstring;
    connect(workerThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    parsePending = false;
    workerThread->start();
}

void HGMarkdownHighlighter::threadFinished()
{
    if (parsePending) {
        this->parse();
        return;
    }

    if (cached_elements != NULL)
        pmh_free_elements(cached_elements);
    cached_elements = workerThread->result;
    workerThread->result = NULL;

    this->highlight();
}

void HGMarkdownHighlighter::handleContentsChange(int position, int charsRemoved,
                                                 int charsAdded)
{
    if (charsRemoved == 0 && charsAdded == 0)
        return;
//    qDebug() << "contents changed. chars removed/added:" << charsRemoved << charsAdded;
    timer->stop();
    timer->start();
}

void HGMarkdownHighlighter::timerTimeout()
{
    this->parse();
}

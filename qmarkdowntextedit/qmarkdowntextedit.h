/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#pragma once

#include <QPlainTextEdit>
#include <QEvent>
#include "qplaintexteditsearchwidget.h"
#include "highlighter.h"

class QMarkdownTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    enum AutoTextOption {
        None = 0x0000,

        // inserts closing characters for brackets and markdown characters
        BracketClosing = 0x0001,

        // removes matching brackets and markdown characters
        BracketRemoval = 0x0002
    };

    Q_DECLARE_FLAGS(AutoTextOptions, AutoTextOption)

    explicit QMarkdownTextEdit(QWidget *parent = nullptr, bool initHighlighter = true);
    HGMarkdownHighlighter *highlighter();
    QPlainTextEditSearchWidget *searchWidget();
    void setIgnoredClickUrlSchemata(QStringList ignoredUrlSchemata);
    virtual void openUrl(QString urlString);
    QString getMarkdownUrlAtPosition(const QString& text, int position);
    void initSearchFrame(QWidget *searchFrame, bool darkMode = false);
    void setAutoTextOptions(AutoTextOptions options);
    static bool isValidUrl(const QString& urlString);
    void resetMouseCursor() const;
    void setReadOnly(bool ro);
    void doSearch(QString &searchText,
                  QPlainTextEditSearchWidget::SearchMode searchMode = QPlainTextEditSearchWidget::SearchMode::PlainTextMode);
    void jumpTo(qlonglong pos);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

public slots:
    void duplicateText();
    void setText(const QString & text);
    void setPlainText(const QString & text);
    void adjustRightMargin();
    void hide();
    bool openLinkAtCursorPosition();
    bool handleBracketRemoval();
    void highlightRichText(pmh_element **result);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    bool increaseSelectedTextIndention(bool reverse);
    bool handleTabEntered(bool reverse);
    QMap<QString, QString> parseMarkdownUrlsFromText(const QString& text);
    bool handleReturnEntered();
    bool handleBracketClosing(const QString& openingCharacter,
                              QString closingCharacter = "");
    bool bracketClosingCheck(const QString& openingCharacter,
                             QString closingCharacter);
    bool quotationMarkCheck(const QString& quotationCharacter);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *event) override;
    HGMarkdownHighlighter *_highlighter;
    QStringList _ignoredClickUrlSchemata;
    QPlainTextEditSearchWidget *_searchWidget;
    QWidget *_searchFrame;
    AutoTextOptions _autoTextOptions;
    QStringList _openingCharacters;
    QStringList _closingCharacters;
    QWidget *lineNumberArea;
    int getDigitsNum() const;
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
signals:
    void urlClicked(QString url);
};

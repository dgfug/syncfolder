//
// Created by faywong on 2020/1/25.
//

#ifndef SYNCFOLDER_LINENUMBERAREA_H
#define SYNCFOLDER_LINENUMBERAREA_H

#include <QtWidgets/QWidget>
#include "qmarkdowntextedit.h"

class LineNumberArea : public QWidget {
public:
    LineNumberArea(QMarkdownTextEdit *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    QMarkdownTextEdit *codeEditor;
};


#endif //SYNCFOLDER_LINENUMBERAREA_H

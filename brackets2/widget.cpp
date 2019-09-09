// completertextedit.cpp
#include "widget.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>
#include <QString>
#include <QTextCursor>

CompleterTextEdit::CompleterTextEdit(QWidget *parent) :
    QTextEdit(parent), m_completer(NULL) {
}

void CompleterTextEdit::setCompleter(QCompleter *completer) {
    m_completer = completer;
    if (m_completer) {
        m_completer->setWidget(this);
        connect(m_completer, SIGNAL(activated(QString)),
                this, SLOT(onCompleterActivated(QString)));
    }
}

void CompleterTextEdit::keyPressEvent(QKeyEvent *e) {
    if (m_completer) {
        if (m_completer->popup()->isVisible()) {
            switch(e->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
                e->ignore();
                return;
            default:
                break;
            }
        }
        QTextEdit::keyPressEvent(e);
        QString completerPrefix = this->wordUnderCursor();
        m_completer->setCompletionPrefix(completerPrefix); // 通过设置QCompleter的前缀，来让Completer寻找关键词
        m_completer->complete();
    }
}

void CompleterTextEdit::onCompleterActivated(const QString &completion) {
    QString completionPrefix = wordUnderCursor(),
            shouldInertText = completion;
    QTextCursor cursor = this->textCursor();
    if (!completion.contains(completionPrefix)) {// delete the previously typed.
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completionPrefix.size());
        cursor.clearSelection();
    } else { // 补全相应的字符
        shouldInertText = shouldInertText.replace(
            shouldInertText.indexOf(completionPrefix), completionPrefix.size(), "");
    }
    cursor.insertText(shouldInertText);
}

QString CompleterTextEdit::wordUnderCursor() { //不断向左移动cursor，并选中字符，并查看选中的单词中是否含有空格——空格作为单词的分隔符
    QTextCursor cursor = this->textCursor();
    while (cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor)) {
        if (cursor.selectedText().contains(" ")) {
            break;
        }
    }
    return cursor.selectedText().remove(" ");
}

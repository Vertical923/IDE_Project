// completertextedit.h
#ifndef COMPLETERTEXTEDIT_H
#define COMPLETERTEXTEDIT_H

#include <QTextEdit>
#include <QString>

QT_BEGIN_NAMESPACE

class QCompleter;

QT_END_NAMESPACE

class CompleterTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CompleterTextEdit(QWidget *parent = 0);
    void setCompleter(QCompleter *completer);

protected:
    void keyPressEvent(QKeyEvent *e); // 响应按键盘事件

private slots:
    void onCompleterActivated(const QString &completion); // 响应选中QCompleter中的选项后，QCompleter发出的activated()信号

private:
    QString wordUnderCursor(); // 获取当前光标所在的单词

private:
    QCompleter *m_completer;

};

#endif // COMPLETERTEXTEDIT_H

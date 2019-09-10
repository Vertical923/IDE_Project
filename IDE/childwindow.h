#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H
#include <QTextEdit>
#include <qtextedit.h>
#include "completelistwidget.h"
#include <QVBoxLayout>
#include <QMainWindow>
#include <QCloseEvent>
#include <QProcess>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QPlainTextEdit>
#include <QResizeEvent>
#include <QSize>
#include <QWidget>
#include <QPlainTextEdit>
#include <QObject>
#include <QPainter>
#include <QListWidget>
#include <QListWidgetItem>
#include <algorithm>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QLineEdit;
class QDialog;
class LineNumberArea;

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class Highlighter : public QSyntaxHighlighter //highlighter的class包含和定义高亮显示关键词的规则
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule //用向量储存highlight的规则
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;//向量

    QRegularExpression commentStartExpression;//需要高亮的关键词
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;//特殊字符的高亮形式
    QTextCharFormat classFormat;//QTextCharFormat提供字符的形式 in a QTextDocument
    QTextCharFormat singleLineCommentFormat;//列出了需要高亮显示的 关键字 的类型
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat valueFormat;
    QTextCharFormat bracketsMatching;//括号匹配里的内容高亮
};

class ChildWindow : public QPlainTextEdit
{
    Q_OBJECT
public:
    void setUpCompleteList();

    explicit ChildWindow(QWidget *parent = 0);
    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QWidget *lineNumberArea;

    QVBoxLayout *mainlayout;

    Highlighter *highlighter;

    QString userFriendlyCurrentFile();
    QString currentFile(){
        return curFile;
    }

    //代码行编号
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    //bianhao
    
protected:
    void closeEvent(QCloseEvent *event);

    //daimahangshu
    void resizeEvent(QResizeEvent *event);

    void keyPressEvent(QKeyEvent *event) override;
    
private slots:
    void documentWasModified();

    //daimahangshu
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    
    void showCompleteWidget();

private:
    void setCurrentFile(const QString &fileName);
    bool maybeSave();
    QString curFile;
    bool isUntitled;

    QColor lineColor;
    QColor editorColor;
    QStringList completeList;//储存自动填充的关键字
    //QListWidget *completeWidget;
    CompleteListWidget *completeWidget;
    QString getWordOfCursor();
    int completeState;
    int getCompleteWidgetX();

};
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(ChildWindow *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    ChildWindow *codeEditor;
};
enum CompleteState{
  Ignore=0,
  Showing=1,
  Hide=2
};
#endif // CHILDWINDOW_H

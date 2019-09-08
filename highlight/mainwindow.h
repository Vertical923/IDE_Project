#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

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
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    void setupEditor();

    QTextEdit *editor;
    Highlighter *highlighter;

    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H

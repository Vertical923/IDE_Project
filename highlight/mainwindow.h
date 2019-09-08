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

class Highlighter : public QSyntaxHighlighter //highlighter��class�����Ͷ��������ʾ�ؼ��ʵĹ���
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule //����������highlight�Ĺ���
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;//����

    QRegularExpression commentStartExpression;//��Ҫ�����Ĺؼ���
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;//�����ַ��ĸ�����ʽ
    QTextCharFormat classFormat;//QTextCharFormat�ṩ�ַ�����ʽ in a QTextDocument
    QTextCharFormat singleLineCommentFormat;//�г�����Ҫ������ʾ�� �ؼ��� ������
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

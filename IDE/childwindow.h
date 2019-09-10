#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H
#include <QTextEdit>
#include <qtextedit.h>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QCloseEvent>
#include <QProcess>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QSyntaxHighlighter>

class QLineEdit;
class QDialog;

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

class ChildWindow : public QTextEdit
{
    Q_OBJECT
public:
    explicit ChildWindow(QWidget *parent = 0);
    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);

    QTextEdit *textedit; // find function needed
    QVBoxLayout *mainlayout;

    Highlighter *highlighter;

    QString userFriendlyCurrentFile();
    QString currentFile(){
        return curFile;
    }
    
protected:
    void closeEvent(QCloseEvent *event);
    
private slots:
    void documentWasModified();
    
private:
    void setCurrentFile(const QString &fileName);
    bool maybeSave();
    QString curFile;
    bool isUntitled;    
};

#endif // CHILDWINDOW_H

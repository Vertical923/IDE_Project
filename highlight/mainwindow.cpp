#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupEditor();

//    setCentralWidget(editor);
//    setWindowTitle(tr("Synax Highlighter"));
}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = this->ui->mytextEdit;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());//���Ӹ�����

    QFile file("mainwindow.h");
    if(file.open(QFile::ReadOnly|QFile::Text))
        editor->setPlainText(file.readAll());
}

MainWindow::~MainWindow()
{
    delete ui;
}

Highlighter::Highlighter(QTextDocument *parent):QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[]={
        QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b")
    };
    //Ϊ ���ϵĹؼ��� ����ר������
    for (const QString &pattern : keywordPatterns){
        rule.pattern=QRegularExpression(pattern);
        rule.format=keywordFormat;
        highlightingRules.append(rule);
    }


    valueFormat.setForeground(Qt::darkYellow);
    const QString valuePatterns[]={
        QStringLiteral("\\breturn\\b"), QStringLiteral("\\btrue\\b"), QStringLiteral("\\bfalse\\b")
    };
    //Ϊ ���ϵĹؼ��� ����ר������
    for (const QString &pattern : valuePatterns){
        rule.pattern=QRegularExpression(pattern);
        rule.format=valueFormat;
        highlightingRules.append(rule);}

    classFormat.setFontWeight(QFont::Bold);//Ϊ ���� �������
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern=QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format=classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);//Ϊ ���� �������
    rule.pattern=QRegularExpression(QStringLiteral("\".*\""));
    rule.format=quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);//Ϊ ������ �������
    functionFormat.setForeground(Qt::blue);
    rule.pattern=QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format=functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);//Ϊ ����ע�� �������
    rule.pattern=QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format=singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);//Ϊ ����ע�� �������

    commentStartExpression=QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression=QRegularExpression(QStringLiteral("\\*/"));
}

void Highlighter::highlightBlock(const QString &text)//Ϊ��/* */��ע����ʽ��ʩ����Ĺ�����ʵ��
{
    for(const HighlightingRule &rule : qAsConst(highlightingRules)){
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()){
            QRegularExpressionMatch match =matchIterator.next();
            setFormat(match.capturedStart(),match.capturedLength(),rule.format);
        }
    }
    setCurrentBlockState(0);
    int startIndex = 0;
    if(previousBlockState()!=1)
        startIndex = text.indexOf(commentStartExpression);
    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else{
            commentLength = endIndex - startIndex+ match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

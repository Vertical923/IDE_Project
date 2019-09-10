#include "childwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPushButton>
#include <qtextedit.h>
#include <qtextcodec.h>
#include <QVBoxLayout>

ChildWindow::ChildWindow(QWidget *parent) :
    QTextEdit(parent)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setAttribute(Qt::WA_DeleteOnClose);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    mainlayout = new QVBoxLayout;

    textedit = new QTextEdit(this);
    textedit->setFont(font);
    highlighter = new Highlighter(textedit->document());
    QFile file("mainwindow.h");
    if(file.open(QFile::ReadOnly|QFile::Text))
        textedit->setPlainText(file.readAll());

    mainlayout->addWidget(this->textedit);
    setLayout(mainlayout);
    textedit->show();
    isUntitled = true;
}

// creat new file
void ChildWindow::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;

    curFile = tr("untitled file%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile+"[*]"+tr("-multifile edit"));

    connect(document() ,SIGNAL(contentsChanged()), this,SLOT(documentWasModified()));
}
// if the file was changed
void ChildWindow::documentWasModified()
{
    setWindowModified(document()->isModified());
}
// the current file
void ChildWindow::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath(); // del the tags in file path
    isUntitled = false;
    document()->setModified(false);

    setWindowTitle(userFriendlyCurrentFile()+"[*]");
}
// show file
bool ChildWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,tr("multifile edit"),
                             tr("cannot read file%1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file); // creat file stream
    QApplication::setOverrideCursor(Qt::WaitCursor); // set the cursor tobe waited
    this->textedit->setPlainText(in.readAll());
    //setPlainText(in.readAll()); // read the file and put them into edit
    QApplication::restoreOverrideCursor(); // set back the station of cursor

    setCurrentFile(fileName);// set the current file
    connect(document(),SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

    return true;

}

QString ChildWindow::userFriendlyCurrentFile()
{
    return QFileInfo(curFile).fileName();
}

void ChildWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

bool ChildWindow::maybeSave()
{
    if(document()->isModified())
    {
        QMessageBox box;
        box.setWindowTitle(tr("multifile edit"));
        box.setText(tr("do you want to save changes of %1").arg(userFriendlyCurrentFile()));
        box.setIcon(QMessageBox::Warning);
        QPushButton *yesBtn = box.addButton(tr("yes"),QMessageBox::YesRole);
        QPushButton *cancelBtn = box.addButton(tr("cancel"), QMessageBox::RejectRole);

        box.exec();
        if(box.clickedButton() == yesBtn)
        {
            return save();
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool ChildWindow::save()
{
    if(isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}
bool ChildWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("save as"), curFile);

    if(fileName.isEmpty())
    {
        return false;
    }
    else
    {
        return saveFile(fileName);
    }
}
bool ChildWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("multifile edit"),
                             tr("cannot save file%1::\n%2.").arg(fileName).arg(file.errorString()));

        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out<<toPlainText();
    QApplication::restoreOverrideCursor();

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
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b"),QStringLiteral("\\b'('\\b")
    };
    //为 以上的关键词 定义专属高亮
    for (const QString &pattern : keywordPatterns){
        rule.pattern=QRegularExpression(pattern);
        rule.format=keywordFormat;
        highlightingRules.append(rule);
    }


    valueFormat.setForeground(Qt::darkYellow);
    const QString valuePatterns[]={
        QStringLiteral("\\breturn\\b"), QStringLiteral("\\btrue\\b"), QStringLiteral("\\bfalse\\b")
    };
    //为 以上的关键词 定义专属高亮
    for (const QString &pattern : valuePatterns){
        rule.pattern=QRegularExpression(pattern);
        rule.format=valueFormat;
        highlightingRules.append(rule);}

    classFormat.setFontWeight(QFont::Bold);//为 类名 定义高亮
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern=QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format=classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);//为 引用 定义高亮
    rule.pattern=QRegularExpression(QStringLiteral("\".*\""));
    rule.format=quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);//为 函数名 定义高亮
    functionFormat.setForeground(Qt::blue);
    rule.pattern=QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format=functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);//为 单行注释 定义高亮
    rule.pattern=QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format=singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);//为 多行注释 定义高亮

    commentStartExpression=QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression=QRegularExpression(QStringLiteral("\\*/"));

    bracketsMatching.setForeground(Qt::green);//为 括号里的内容 定义高亮
    rule.pattern=QRegularExpression(QStringLiteral("(\\().*?(\\))"));
    rule.format=bracketsMatching;
    highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)//为“/* */”注释形式设施特殊的功能以实现
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

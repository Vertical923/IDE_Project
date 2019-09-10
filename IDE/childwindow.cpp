#include "childwindow.h"
#include <QFile>
#include <qplaintextedit.h>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QtWidgets>
#include <QApplication>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPushButton>
#include <qtextedit.h>
#include <qtextcodec.h>
#include <QVBoxLayout>
#include <QPlainTextEdit>

ChildWindow::ChildWindow(QWidget *parent) :
    QPlainTextEdit(parent)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setAttribute(Qt::WA_DeleteOnClose);
    mainlayout = new QVBoxLayout;

    lineNumberArea = new LineNumberArea(this);
    lineNumberArea->setFont(font);

    highlighter = new Highlighter(this->document());
    QFile file("mainwindow.h");
    if(file.open(QFile::ReadOnly|QFile::Text))
        this->setPlainText(file.readAll());

    connect(this, SIGNAL(blockCountChanged(int)), this,SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    isUntitled = true;
}
// tmf
int ChildWindow::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}
void ChildWindow::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ChildWindow::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ChildWindow::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
    {
        lineNumberArea->scroll(0, dy);
    }
    else
    {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }
    if(rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(0);
    }
}

void ChildWindow::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}
void ChildWindow::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
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
    this->setPlainText(in.readAll());
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

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

ChildWindow::ChildWindow(QWidget *parent) :
    QTextEdit(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

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
    setPlainText(in.readAll()); // read the file and put them into edit
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

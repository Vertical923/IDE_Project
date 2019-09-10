#include "ide.h"
#include "ui_ide.h"
#include "childwindow.h"
#include "qmdisubwindow.h"
#include "stdlib.h"
#include <QApplication>
#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <qtextedit.h>

IDE::IDE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IDE)
{
    ui->setupUi(this);

    isUntitled = true;    // 初始化文件为未保存状态
    curFile = tr("zhtql");    // 初始化文件名为"未命名.c"
    setWindowTitle(curFile);    // 初始化窗口标题为文件名

    /*多文档子窗口模式*/
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);

    /*创建查找窗口*/
    findDlg = new QDialog(this);
    findDlg->setWindowTitle(tr("查找"));
    findLineEdit = new QLineEdit(findDlg);
    QPushButton *btn= new QPushButton(tr("查找下一个"), findDlg);
    QVBoxLayout *layout= new QVBoxLayout(findDlg);
    layout->addWidget(findLineEdit);
    layout->addWidget(btn);

    connect(btn, SIGNAL(clicked()), this, SLOT(showFindText()));
}

IDE::~IDE()
{
    delete ui;
}

ChildWindow * IDE::activeChildwindow()
{
    if(QMdiSubWindow * activeSubwindow = ui->mdiArea->activeSubWindow())
    {
        return qobject_cast<ChildWindow *>(activeSubwindow->widget());
        return 0;
    }
}

//打开，加载文件
bool IDE::loadFile(const QString &fileName)
{
   QFile file(fileName); // 新建QFile对象
   if (!file.open(QFile::ReadOnly | QFile::Text))
   {
       QMessageBox::warning(this, tr("多文档编辑器"),tr("无法读取文件 %1:\n%2.").arg(fileName).arg(file.errorString()));
       return false; // 只读方式打开文件，出错则提示，并返回false
   }
   QTextStream in(&file); // 新建文本流对象
   QApplication::setOverrideCursor(Qt::WaitCursor);

   // 读取文件的全部文本内容，并添加到编辑器中
   //ui->textEdit->setPlainText(in.readAll());
   //activeChildwindow()->lineNumberArea->setPlainText(in.readAll());

   //setPlainText(in.readAll());
   activeChildwindow()->setPlainText(in.readAll());
   QApplication::restoreOverrideCursor();

   // 设置当前文件
   curFile = QFileInfo(fileName).canonicalFilePath();
   setWindowTitle(curFile);
   return true;

}
QMdiSubWindow * IDE::findChildWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        ChildWindow *mdiChild = qobject_cast<ChildWindow *>(window->widget());
        if(mdiChild->currentFile() == canonicalFilePath)
        {
            loadFile(fileName);
        }
    }
    return 0;
}

//执行真正的文件保存操作
bool IDE::saveFile(const QString &fileName)
{
   QFile file(fileName);

   if (!file.open(QFile::WriteOnly | QFile::Text))
   {
       QMessageBox::warning(this, tr("多文档编辑器"),tr("无法写入文件 %1：/n %2").arg(fileName).arg(file.errorString()));// %1和%2分别对应后面arg两个参数，/n起换行的作用
       return false;
   }
   QTextStream out(&file);
   QApplication::setOverrideCursor(Qt::WaitCursor);// 鼠标指针变为等待状态
   out << activeChildwindow()->toPlainText();
   QApplication::restoreOverrideCursor();   // 鼠标指针恢复原来的状态

   isUntitled = false;
   curFile = QFileInfo(fileName).canonicalFilePath();   // 获得文件的标准路径
   setWindowTitle(curFile);

   return true;
}

//saveAs()函数
//这里使用QFileDialog来实现了一个另存为对话框，并且获取了文件的路径，然后使用文件路径来保存文件。
bool IDE::saveAs()
{
   QString fileName = QFileDialog::getSaveFileName(this,tr("另存为"),"未命名.c");
   if (fileName.isEmpty())
       return false;
   return saveFile(fileName);
}

//保存函数
//如果保存过则执行另存为saveAs()，没有则执行save()
bool IDE::save()
{
   if (isUntitled)//判断有没有被保存过
   {
       return saveAs();
   }
   else
   {
       return saveFile(curFile);
   }
}

//判断文档是否需要保存
bool IDE::maybeSave()
{
    // 如果文档被更改了
    if(activeChildwindow()->document()->isModified()) {
    // 自定义一个警告对话框
       QMessageBox box;
       box.setWindowTitle(tr("警告"));
       box.setIcon(QMessageBox::Warning);
       box.setText(curFile + tr(" 尚未保存，是否保存？"));

       QPushButton *yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
       box.addButton(tr("否(&N)"), QMessageBox::NoRole);
       QPushButton *cancelBut = box.addButton(tr("取消"),QMessageBox::RejectRole);
       box.exec();
       if (box.clickedButton() == yesBtn)
            return save();
       else if (box.clickedButton() == cancelBut)
            return false;
   }
   // 如果文档没有被更改，则直接返回true
   return true;
}

//关闭
void IDE::closeEvent(QCloseEvent *event)
{
   if (maybeSave()) // 如果maybeSave()函数返回true，则关闭程序
   {
       event->accept();
   }
   else
   {
       event->ignore();// 否则忽略该事件
   }
}

//新建文件操作函数
void IDE::newFile()
{
    ChildWindow *child = creatChild();
    child->newFile();
    child->show();
}
ChildWindow * IDE::creatChild()
{
    ChildWindow *child = new ChildWindow;
    //ui->mdiArea->addSubWindow(child);
    ui->mdiArea->addSubWindow(child);
    connect(child,SIGNAL(copyAvailable(bool)),ui->action_X,
            SLOT(setEnabled(bool)));

    connect(child,SIGNAL(copyAvailable(bool)),ui->action_C,
            SLOT(setEnabled(bool)));

    connect(child->document(),SIGNAL(undoAvailable(bool)),
            ui->action_Z,SLOT(setEnabled(bool)));

    connect(child,SIGNAL(cursorPositionChanged()),this,SLOT(showTextRowAndCol()));

    return child;
}

//新建
void IDE::on_action_N_triggered()
{
    newFile();
}

//保存
void IDE::on_action_S_triggered()
{
    if(activeChildwindow() && activeChildwindow()->save())
        ui->statusBar->showMessage(tr("saved"));
}


//打开
void IDE::on_action_O_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        QMdiSubWindow *existing = findChildWindow(fileName);
        if(existing)
        {
            ui->mdiArea->setActiveSubWindow(existing);
            return;
        }
        ChildWindow *child = creatChild();
        if(child->loadFile(fileName))
        {
            ui->statusBar->showMessage(tr("opened"),2000);
            child->show();
        }
        else
        {
            child->close();
        }
    }
}

//关闭
void IDE::on_action_W_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

//退出
void IDE::on_action_Q_triggered()
{
    //on_action_Q_triggered();// 先执行关闭操作，再退出程序，qApp是指向应用程序的全局指针
    qApp->quit();
}

//撤销
void IDE::on_action_Z_triggered()
{
    if(activeChildwindow())
        activeChildwindow()->undo();
}

//剪切
void IDE::on_action_X_triggered()
{
    if(activeChildwindow())
        activeChildwindow()->cut();
}

//复制
void IDE::on_action_C_triggered()
{
    if(activeChildwindow())
        activeChildwindow()->redo();
}

//粘贴
void IDE::on_action_V_triggered()
{
    if(activeChildwindow())
        activeChildwindow()->paste();
}

//编译槽函数
void IDE::on_action_I_triggered()
{
    //判断是否需要保存
    activeChildwindow()->save();
    //save();
    //编译
    compile();
}

//运行
void IDE::on_action_R_triggered()
{
    //判断是否需要保存
    activeChildwindow()->save();
    //save();
    //编译
    compile();
    //运行
    if (compile_success)
    {
        //获取执行文件目录
        QString path = curFile.left(curFile.length() - 2)+".exe";
        path.replace("/","\\");//替换
        qDebug()<<path<<endl;
        QProcess cmd;
        cmd.start("cmd.exe",QStringList()<<"/c"<<QString("start " + path));
        cmd.waitForStarted(50);
        cmd.waitForFinished();
        cmd.close();
    }
}

void IDE::read_error()
{
    QByteArray bytes = cmd->readAllStandardError();

    QString result = QString::fromLocal8Bit(bytes);
    compile_success = false;
    //qDebug()<<result<<endl;
    ui->textEdit_2->setText(result);
}

void IDE::showFindText()
{
    QString str = findLineEdit->text();//获取编译器中要查找的字符串
    //activeChildwindow()->lineNumberArea->find(str, QTextDocument::FindBackward);
    activeChildwindow()->find(str, QTextDocument::FindBackward);
    //activeChildwindow()->textedit->find(str, QTextDocument::FindBackward);
    //未查找到则返回提示
    if (!activeChildwindow()->find(str, QTextDocument::FindBackward))
    {
       QMessageBox::warning(this, tr("查找"),tr("找不到%1").arg(str));
    }
}

//查找
void IDE::on_action_F_triggered()
{
    findDlg->show();
}

//另存为
void IDE::on_action_M_triggered()
{
    saveAs();
}

//编译
void IDE::compile()
{
    ui->textEdit_2->clear();
    compile_success = true;//初始化为true
    cmd = new QProcess();
    QByteArray command;
    connect(cmd,SIGNAL(readyReadStandardError()),this,SLOT(read_error()));//初始化

    cmd ->start("cmd.exe");//运行进程

    //获取文件路径
    QString file_name;

    file_name = curFile.right(curFile.length() - curFile.lastIndexOf("/") - 1);
    QString path_str = curFile.left(curFile.lastIndexOf("/")); //程序所在文件路径，不包括文件名
    path_str.replace("/","\\");//替换
    QString disk = path_str.left(2);//找出文件所在磁盘

    QString cmd_change_disk = disk + "\r\n";
    command = cmd_change_disk.toLocal8Bit();
    cmd->write(command);//换磁盘

    QString cmd_change_path ="cd "+path_str+"\r\n";
    command = cmd_change_path.toLocal8Bit();
    cmd->write(command);//进入所在文件夹

    QString gcc_c ="gcc "+ file_name +" -o "+file_name.left(file_name.length() - 2) + "\r\n";
    command = gcc_c.toLocal8Bit();
    cmd ->write(command);//编译

    QString exit = "exit";
    command = exit.toLocal8Bit();
    cmd ->write(command);

    cmd->waitForFinished(100);
    cmd->close();

}

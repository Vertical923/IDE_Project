#ifndef IDE_H
#define IDE_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMdiArea>
#include <qtextedit.h>
class ChildWindow;
class QMdiSubWindow;
class QSingalMapper;
class QLineEdit;
class QDialog;

namespace Ui {
class IDE;
}

class IDE : public QMainWindow
{
    Q_OBJECT

public:
    QMdiArea *mdiArea;
    explicit IDE(QWidget *parent = 0);
    void newFile();   // 新建操作
    bool maybeSave(); // 判断是否需要保存
    bool save();      // 保存操作
    bool saveAs();    // 另存为操作
    bool saveFile(const QString &fileName); // 保存文件
    bool loadFile(const QString &fileName); // 加载文件
    ~IDE();

private slots:
    void on_action_N_triggered();

    void on_action_S_triggered();

    void on_action_O_triggered();

    void on_action_W_triggered();

    void on_action_Q_triggered();

    void on_action_Z_triggered();

    void on_action_X_triggered();

    void on_action_C_triggered();

    void on_action_V_triggered();

    void showFindText();//查找窗口

    void on_action_F_triggered();

    void on_action_M_triggered();
    
    // creat child window
    ChildWindow *creatChild();
    
private:
    Ui::IDE *ui;

    bool isUntitled;// 为真表示文件没有保存过，为假表示文件已经被保存过了
    QString curFile;// 保存当前文件的路径

    QLineEdit *findLineEdit;//查找
    QDialog *findDlg;

    QSingalMapper *windowMapper;
    ChildWindow *activeChildwindow(); // creat child window
    QMdiSubWindow *findChildWindow(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event); // 关闭事件
};

#endif // IDE_H

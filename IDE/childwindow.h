#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H
#include <QTextEdit>
#include <qtextedit.h>
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

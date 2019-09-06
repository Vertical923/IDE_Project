#include "ide.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IDE w;
    w.show();
    //保证代码中可以添加中文字符
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    return a.exec();
}

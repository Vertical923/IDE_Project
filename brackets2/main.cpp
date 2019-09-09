// main.cpp
#include <QApplication>
#include <QCompleter>
#include <QLineEdit>
#include <QStringList>

#include "widget.h"

int main(int ac, char **av) {
    QApplication app(ac, av);

    CompleterTextEdit *textEdit = new CompleterTextEdit;
    QStringList stringList;
    for (int index = 1; index <= 1; ++index) {
        stringList << QString("()").arg(index)<<QString("{}").arg(index)<<QString("[]").arg(index);
    }
    QCompleter *completer = new QCompleter(stringList, textEdit);
    textEdit->setCompleter(completer);
    textEdit->show();


    return app.exec();
}

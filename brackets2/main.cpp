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
        stringList << QString("()")<<QString("{}")<<QString("[]")<<QString("char")
                      <<QString("class")<<QString("const")<<QString("double")<<QString("enum")
                      <<QString("enum")<<QString("explicit")<<QString("friend")<<QString("inline")
                      <<QString("int")<<QString("long")<<QString("namespace")<<QString("operator")
                      <<QString("private")<<QString("protected")<<QString("public")<<QString("short")
                      <<QString("signals")<<QString("signed")<<QString("slots")<<QString("static")
                      <<QString("struct")<<QString("template")<<QString("typedef")<<QString("typename")
                      <<QString("union")<<QString("unsigned")<<QString("virtual")<<QString("void")
                      <<QString("volatile")<<QString("bool");
    }
    QCompleter *completer = new QCompleter(stringList, textEdit);
    textEdit->setCompleter(completer);
    textEdit->show();


    return app.exec();
}

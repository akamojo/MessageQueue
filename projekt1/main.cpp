#include "widget.h"
#include <QApplication>
#include "clilib.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setFixedSize(w.size());
    w.show();

    return a.exec();
}

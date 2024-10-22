#include "sctcptoolwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SCTcpToolWidget w;
    w.show();
    return a.exec();
}

#include "Output.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Output w;
    w.show();
    return a.exec();
}

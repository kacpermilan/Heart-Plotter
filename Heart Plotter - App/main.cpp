#include "HeartPlotterApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HeartPlotterApp w;
    w.show();
    return a.exec();
}

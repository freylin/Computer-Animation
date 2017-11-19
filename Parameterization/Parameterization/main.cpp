#include "parameterization.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Parameterization w;
    w.show();
    return a.exec();
}

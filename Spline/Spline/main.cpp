#include "spline.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    spline w;
    w.show();

    return a.exec();
}

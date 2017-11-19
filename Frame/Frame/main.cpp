#include "frame.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frame w;
    w.show();

    return a.exec();
}

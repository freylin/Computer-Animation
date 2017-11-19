#include "imagemorphing.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageMorphing w;
    w.show();

    return a.exec();
}

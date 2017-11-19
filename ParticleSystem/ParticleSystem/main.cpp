#include "particlesystem.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParticleSystem w;
    w.show();

    return a.exec();
}

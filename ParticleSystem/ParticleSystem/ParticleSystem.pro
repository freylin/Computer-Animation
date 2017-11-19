#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T16:35:31
#
#-------------------------------------------------

QT       += core gui
QT       += opengl
LIBS     += -lglut32 -lglu32 -lopengl32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ParticleSystem
TEMPLATE = app


SOURCES += main.cpp\
        particlesystem.cpp

HEADERS  += particlesystem.h

FORMS    +=

RESOURCES += \
    pix.qrc

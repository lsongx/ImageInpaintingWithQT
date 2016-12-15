#-------------------------------------------------
#
# Project created by QtCreator 2016-12-07T15:47:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt_inpainting
TEMPLATE = app


SOURCES +=\
    src/main.cpp \
    src/windowAbout.cpp \
    src/windowCaptureWebcam.cpp \
    src/windowDo4.cpp \
    src/windowImage.cpp \
    src/windowMain.cpp \
    src/windowPreferences.cpp \
    src/windowStartup.cpp \
    src/inpainting_algo.cpp \
    src/utils.cpp

HEADERS  += \
    src/windowAbout.h \
    src/windowCaptureWebcam.h \
    src/windowDo4.h \
    src/windowImage.h \
    src/windowMain.h \
    src/windowPreferences.h \
    src/windowStartup.h \
    src/inpainting_algo.h \
    src/utils.hpp

FORMS    += \
    ui/windowAbout.ui \
    ui/windowCaptureWebcam.ui \
    ui/windowDo4.ui \
    ui/windowImage.ui \
    ui/windowMain.ui \
    ui/windowPreferences.ui \
    ui/windowStartup.ui \
    ui/barCriminisi.ui \
    ui/barPDE_NS.ui \
    ui/barHe.ui \
    ui/barTELEA.ui

win32: LIBS += -L$$PWD/../../../opencv/build/x64/vc14/lib/ -lopencv_world310

INCLUDEPATH += $$PWD/../../../opencv/build/include
DEPENDPATH += $$PWD/../../../opencv/build/include

RESOURCES += \
    icon_src.qrc

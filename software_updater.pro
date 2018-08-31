#-------------------------------------------------
#
# Project created by QtCreator 2017-10-03T11:42:35
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = updater
TEMPLATE = app

QMAKE_LFLAGS+=-static-libstdc++
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=  main.cpp\
            widget.cpp \
            styles/styles.cpp \
    helper.cpp

HEADERS  += widget.h \
            styles/styles.h \
    helper.h


# UMU firmware programmer

SOURCES += ../umu-firmware-updater/programengine.cpp \
           ../umu-firmware-updater/ftpdev.cpp \
           ../umu-firmware-updater/stringlist.cpp \
           ../umu-firmware-updater/qftp/qftp.cpp

HEADERS += ../umu-firmware-updater/programengine.h \
           ../umu-firmware-updater/ftpdev.h \
           ../umu-firmware-updater/stringlist.h \
           ../umu-firmware-updater/qftp/qftp.h

INCLUDEPATH += ../umu-firmware-updater/qftp \
               ../umu-firmware-updater


FORMS    += widget.ui \
    helper.ui

linux-imx51-g++ | linux-buildroot-g++{
    DEFINES += IMX_DEVICE
    target.path = /usr/local/avicon-31/updater
    INSTALLS += target
}

RESOURCES += resources.qrc

TRANSLATIONS = russian.ts

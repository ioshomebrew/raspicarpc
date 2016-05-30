#-------------------------------------------------
#
# Project created by QtCreator 2015-07-22T16:03:50
#
#-------------------------------------------------

QT       += core gui bluetooth dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CarPlay
TEMPLATE = app
target.path = /root/CarPlay
INSTALLS += target


SOURCES += main.cpp\
        mainwindow.cpp \
    nowplaying.cpp \
    scrolltext.cpp \
    phone.cpp \
    phonecallview.cpp \
    WidgetMarqueeLabel.cpp \
    bluesettings.cpp \
    settings.cpp \
    audiosettings.cpp

HEADERS  += mainwindow.h \
    nowplaying.h \
    scrolltext.h \
    phone.h \
    phonecallview.h \
    WidgetMarqueeLabel.h \
    bluesettings.h \
    settings.h \
    audiosettings.h

FORMS    += mainwindow.ui \
    nowplaying.ui \
    phone.ui \
    phonecallview.ui \
    bluesettings.ui \
    settings.ui \
    audiosettings.ui

DISTFILES += \
    README

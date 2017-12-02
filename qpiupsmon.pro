TEMPLATE = app
QT = core network

TARGET = qpiupsmon
DESTDIR = build

CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ext/inc
LIBS += -Lext/lib -lqmqtt
LIBS += -Lext/lib -lbcm2835

SOURCES += \
    main.cpp \
    qpiupsmon.cpp

HEADERS += \
    qpiupsmon.h

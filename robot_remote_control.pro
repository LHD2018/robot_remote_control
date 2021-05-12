QT       += core gui webenginewidgets network qmqtt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

RC_ICONS = logo.ico

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    camimgthread.cpp \
    imgtrancefer.cpp \
    joystick.cpp \
    joythread.cpp \
    lidarimgthread.cpp \
    main.cpp \
    mainwindow.cpp \
    mqttdialog.cpp \
    mqttpubsub.cpp \
    mysocket.cpp \
    socketworker.cpp \
    speeddial.cpp \
    utils.cpp

HEADERS += \
    aboutdialog.h \
    camimgthread.h \
    imgtrancefer.h \
    joystick.h \
    joythread.h \
    lidarimgthread.h \
    mainwindow.h \
    mqttdialog.h \
    mqttpubsub.h \
    mysocket.h \
    socketworker.h \
    speeddial.h \
    utils.h

FORMS += \
    aboutdialog.ui \
    mainwindow.ui \
    mqttdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $${PWD}/ffmpeg/include

LIBS += -lws2_32

LIBS += -L$${PWD}/ffmpeg/lib \
    -lavcodec \
    -lavutil \
    -lpostproc \
    -lswresample \
    -lswscale \
    -lavdevice \
    -lavfilter \
    -lavformat

RESOURCES += \
    res.qrc



QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Battery.cpp \
    Bolus.cpp \
    CGM.cpp \
    Cartridge.cpp \
    Device.cpp \
    Display.cpp \
    Person.cpp \
    Profile.cpp \
    Pump.cpp \
    Simulator.cpp \
    abstractpage.cpp \
    homepage.cpp \
    main.cpp \
    ProfileSegment.cpp \
    mainwindow.cpp \
    manualbolus1.cpp \
    mypump1.cpp \
    options1.cpp \
    pagewindow.cpp \
    personalprofiles1.cpp \
    mytime.cpp \
    personalprofiles2.cpp \
    sleepscreen.cpp \
    status1.cpp \
    qcustomplot.cpp

HEADERS += \
    Battery.h \
    Bolus.h \
    CGM.h \
    Cartridge.h \
    ControlIQ.h \
    Device.h \
    DeviceStates.h \
    Display.h \
    Person.h \
    Profile.h \
    Pump.h \
    Simulator.h \
    ProfileSegment.h \
    defs.h \
    mainwindow.h \
    manualbolus1.h \
    mypump1.h \
    mytime.h \
    pagewindow.h \
    homepage.h \
    abstractpage.h \
    homepage.h \
    mainwindow.h \
    options1.h \
    pagewindow.h \
    personalprofiles1.h \
    mytime.h \
    personalprofiles2.h \
    sleepscreen.h \
    status1.h \
    qcustomplot.h

FORMS += \
    homepage.ui \
    mainwindow.ui \
    manualbolus1.ui \
    mypump1.ui \
    options1.ui \
    pagewindow.ui \
    personalprofiles1.ui \
    personalprofiles2.ui \
    sleepscreen.ui \
    status1.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

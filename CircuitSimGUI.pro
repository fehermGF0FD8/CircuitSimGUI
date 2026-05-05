QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += src/gui src/core

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/gui/addcircuitdialog.cpp \
    src/core/builtincomponent.cpp \
    src/core/circuitview.cpp \
    main.cpp \
    src/gui/mainwindow.cpp \
    src/core/resistor.cpp

HEADERS += \
    src/gui/addcircuitdialog.h \
    src/core/builtincomponent.h \
    src/core/circuitview.h \
    src/gui/mainwindow.h \
    src/core/resistor.h

FORMS += \
    forms/addcircuitdialog.ui \
    forms/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

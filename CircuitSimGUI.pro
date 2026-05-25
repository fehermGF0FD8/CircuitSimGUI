QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += src/gui src/core src/core/entities src/core/graphics

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/core/circuit.cpp \
    src/core/graphics/componentpainter.cpp \
    src/core/entities/conductance.cpp \
    src/core/entities/currentsource.cpp \
    src/core/entities/voltagesource.cpp \
    src/core/idmanager.cpp \
    src/core/entities/capacitor.cpp \
    src/core/networktopology.cpp \
    src/core/node.cpp \
    src/core/pin.cpp \
    src/core/projectmanager.cpp \
    src/core/wire.cpp \
    src/gui/addcircuitdialog.cpp \
    src/core/builtincomponent.cpp \
    src/gui/circuitview.cpp \
    main.cpp \
    src/gui/mainwindow.cpp \
    src/core/entities/resistor.cpp

HEADERS += \
    src/core/circuit.h \
    src/core/entities/entities.h \
    src/core/graphics/componentpainter.h \
    src/core/componenttype.h \
    src/core/entities/conductance.h \
    src/core/entities/currentsource.h \
    src/core/entities/voltagesource.h \
    src/core/idmanager.h \
    src/core/entities/capacitor.h \
    src/core/networktopology.h \
    src/core/node.h \
    src/core/nodetype.h \
    src/core/pin.h \
    src/core/projectmanager.h \
    src/core/snapping.h \
    src/core/wire.h \
    src/gui/addcircuitdialog.h \
    src/core/builtincomponent.h \
    src/gui/circuitview.h \
    src/gui/mainwindow.h \
    src/core/entities/resistor.h

FORMS += \
    forms/addcircuitdialog.ui \
    forms/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

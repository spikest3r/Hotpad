QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_FILE = app.rc
CONFIG+=release

SOURCES += \
    datafile.cpp \
    globals.cpp \
    hook.cpp \
    main.cpp \
    window.cpp

HEADERS += \
    actions.h \
    datafile.h \
    globals.h \
    hook.h \
    window.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    app.rc \
    icon.ico

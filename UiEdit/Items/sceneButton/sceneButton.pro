QT+= core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
CONFIG += c++17
CONFIG += shared

DEFINES += _ITEM
#DEFINES += QT_NO_DEBUG_OUTPUT
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -U__STRICT_ANSI__


#mingw
win32-g++{
   BINDIR = $$PWD/../../../win32-gcc-mes-bin
}
#msvc
win32-msvc{
   BINDIR = $$PWD/../../../win32-msvc-mes-bin
}

#arm-linux
linux-arm-g++{
    BINDIR = $$PWD/../../arm-linux-gcc-bin
    DEFINES += __NO_PRECOMPILED_HEADER__
    DEFINES += _TTY_POSIX_
}

CONFIG(debug, debug|release){
    DESTDIR = $$BINDIR/debug/Items
    libPath = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release/Items
    libPath = $$BINDIR/release
}

LIBS += -L$$libPath

#App
LIBS += -L$$libPath -lApp
#Interface
INCLUDEPATH +=  $$PWD/../../../Interface

TARGET = sceneButton

HEADERS += \
    ../BackgroundSettingsW.h \
    ../ItemProEditDlgBase.h \
    ../LabelBase.h \
    ../item_base.h \
    sceneButton.h \
    sceneButtonProDlg.h

SOURCES += \
    ../BackgroundSettingsW.cpp \
    ../ItemProEditDlgBase.cpp \
    ../LabelBase.cpp \
    sceneButton.cpp \
    sceneButtonProDlg.cpp

INCLUDEPATH +=  ../../../MesHost/MesHost/Interface

RESOURCES += \
    image.qrc






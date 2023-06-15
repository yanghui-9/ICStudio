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

TARGET = bitButton

HEADERS += \
    ../ItemProEditDlgBase.h \
    ../item_base.h \
    bitButton.h \
    bitButtonProDlg.h

SOURCES += \
    ../ItemProEditDlgBase.cpp \
    bitButton.cpp \
    bitButtonProDlg.cpp


#Interface
INCLUDEPATH +=  $$PWD/../../../Interface

DISTFILES += \
    Bit Button.png \
    Bit Button.png

RESOURCES += \
    image.qrc






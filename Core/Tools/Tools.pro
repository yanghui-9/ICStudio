QT += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += shared

DEFINES += TOOLS_LIB


win32-msvc{
   BINDIR = $$PWD/../../win32-msvc-mes-bin
}

CONFIG(debug, debug|release){
    DESTDIR = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release
}

#CONFIG += PRECOMPILED_HEADER
#Use Precompiled headers (PCH)
#PRECOMPILED_HEADER = cstruct_pch_headers.h

TARGET = Tools

HEADERS += \
    ErrCodeHelp.h \
    SqlManagement.h \
    checknetworkconnection.h \
    excel.h \
    tools.h

SOURCES += \
    ErrCodeHelp.cpp \
    SqlManagement.cpp \
    checknetworkconnection.cpp \
    excel.cpp \
    tools.cpp

#MESGLOG
    LIBS += -L$$PWD/../../3rdparty/glog/lib/ -lglog
    INCLUDEPATH += $$PWD/../../3rdparty/glog/include

QT += core
TARGET = test1
TEMPLATE = lib
CONFIG += c++17
DEFINES += API_FUNC


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


win32-msvc{
   BINDIR = $$PWD/../../win32-msvc-mes-bin
}

CONFIG(debug, debug|release){
    DESTDIR = $$BINDIR/debug/Func/
    libPath = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release/Func/
    libPath = $$BINDIR/release
}


HEADERS +=     test1.h

SOURCES +=     test1.cpp

#GLOG
LIBS += -L$$PWD/../../3rdparty/glog/lib/ -lglog
INCLUDEPATH += $$PWD/../../3rdparty/glog/include
#App
LIBS += -L$$libPath -lApp
#Interface
INCLUDEPATH +=  $$PWD/../../Interface
#Lua
LIBS += -L$$PWD/../../3rdparty/lua/lib/ -llua
INCLUDEPATH += $$PWD/../../3rdparty/lua/include


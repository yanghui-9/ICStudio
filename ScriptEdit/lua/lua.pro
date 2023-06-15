#-------------------------------------------------
#
# Project created by QtCreator 2017-12-09T14:36:16
#
#-------------------------------------------------
QT += core
TARGET = lua

TEMPLATE = lib
CONFIG += shared
DEFINES += LUA_LIBRARY

#Mingw
win32-g++{
    BINDIR = ../../win32-gcc-bin-new
}

win32-msvc{
    BINDIR = $$PWD/../../win32-msvc-mes-bin
}

#arm-linux
linux-arm-g++{
    BINDIR = ../../../arm-linux-gcc-bin

    DEFINES += __NO_PRECOMPILED_HEADER__
    DEFINES += _TTY_POSIX_
}

CONFIG(debug, debug|release){
    DESTDIR = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release
}

SOURCES += \
    src/lapi.c \
    src/lauxlib.c \
    src/lbaselib.c \
    src/lbitlib.c \
    src/lcode.c \
    src/lcorolib.c \
    src/lctype.c \
    src/ldblib.c \
    src/ldebug.c \
    src/ldo.c \
    src/ldump.c \
    src/lfunc.c \
    src/lgc.c \
    src/linit.c \
    src/liolib.c \
    src/llex.c \
    src/lmathlib.c \
    src/lmem.c \
    src/loadlib.c \
    src/lobject.c \
    src/lopcodes.c \
    src/loslib.c \
    src/lparser.c \
    src/lstate.c \
    src/lstring.c \
    src/lstrlib.c \
    src/ltable.c \
    src/ltablib.c \
    src/ltm.c \
    src/lundump.c \
    src/lutf8lib.c \
    src/lvm.c \
    src/lzio.c

HEADERS +=\
    src/lapi.h \
    src/lauxlib.h \
    src/lcode.h \
    src/lctype.h \
    src/ldebug.h \
    src/ldo.h \
    src/lfunc.h \
    src/lgc.h \
    src/llex.h \
    src/llimits.h \
    src/lmem.h \
    src/lobject.h \
    src/lopcodes.h \
    src/lparser.h \
    src/lprefix.h \
    src/lstate.h \
    src/lstring.h \
    src/ltable.h \
    src/ltm.h \
    src/lua.h \
    src/lua.hpp \
    src/luaconf.h \
    src/lualib.h \
    src/lundump.h \
    src/lvm.h \
    src/lzio.h


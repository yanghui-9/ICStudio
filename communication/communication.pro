QT -= gui
CONFIG += shared
TARGET = communication
TEMPLATE = lib
#CONFIG += c++11
DEFINES += COMMUNICATION_LIBRARY


DEFINES += ETHERNET
DEFINES += SERIAL

#Mingw
win32-g++{
    BINDIR = $$PWD/../win32-gcc-bin
    LIBS +=   -lWs2_32
    LIBS +=  libsetupapi
}

win32-msvc{
   BINDIR = $$PWD/../win32-msvc-mes-bin
   LIBS +=   -lWs2_32
   LIBS += advapi32.lib
   LIBS += setupapi.lib
}

#arm-linux
linux-arm-g++{
    BINDIR = $$PWD/../arm-linux-gcc-bin
    DEFINES += __NO_PRECOMPILED_HEADER__
    DEFINES += _TTY_POSIX_
}

CONFIG(debug, debug|release){
    DESTDIR = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release
}


SOURCES += \
    Communication.cpp \
    LogService.cpp \
    TcpClient.cpp \
    TcpServer.cpp \
    UpdComm.cpp \
    socket/io_log_service.cpp \
    socket/io_select.cpp \
    socket/io_util.cpp
HEADERS += \
    ../host_new/base_include/interface_comm.h \
    LogService.h \
    SafeQueue.h \
    TcpClient.h \
    TcpServer.h \
    UpdComm.h \
    communicationdef.h \
    communicationglobal.h \
    socket/io_def.h \
    socket/io_log_service.h \
    socket/io_select.h \
    socket/io_util.h


if(contains(DEFINES,SERIAL))
{

SOURCES += SerialComm.cpp \
    serialport/SerialPort.cpp \
    serialport/SerialPortBase.cpp \
    serialport/SerialPortInfo.cpp \
    serialport/SerialPortInfoBase.cpp

HEADERS +=\
    SerialComm.h \
    serialport/osplatformutil.h \
    serialport/SerialPort.h \
    serialport/SerialPort_global.h \
    serialport/SerialPortBase.h \
    serialport/SerialPortInfo.h \
    serialport/SerialPortInfoBase.h

unix:HEADERS           += serialport/SerialPortInfoUnixBase.h
unix:HEADERS           += serialport/SerialPortUnixBase.h
unix:SOURCES           += serialport/SerialPortInfoUnixBase.cpp
unix:SOURCES           += serialport/SerialPortUnixBase.cpp


win32:HEADERS          += serialport/SerialPortInfoWinBase.h
win32:HEADERS          += serialport/SerialPortWinBase.h
win32:SOURCES          += serialport/SerialPortInfoWinBase.cpp
win32:SOURCES          += serialport/SerialPortWinBase.cpp

}

#if(contains(DEFINES,ETHERNET))
#{
#}


QT += core gui network concurrent #xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets axcontainer

TARGET = ICStudio
TEMPLATE = app
RC_FILE += logo.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS GLOG_NO_ABBREVIATED_SEVERITIES

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


win32-msvc{
   BINDIR = $$PWD/../../win32-msvc-mes-bin
}
CONFIG(debug, debug|release){
    DESTDIR = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release
}

include(FileCommunication/FileCommunication.pri)
include(Process/Process.pri)


#NLOHMANN_JSON
    INCLUDEPATH +=  $$PWD/../../3rdparty/json-develop/include

#MESGLOG
    LIBS += -L$$PWD/../../3rdparty/glog/lib/ -lglog
    INCLUDEPATH += $$PWD/../../3rdparty/glog/include

#App
LIBS += -L$$DESTDIR -lApp

#Tools
LIBS += -L$$DESTDIR -lTools
INCLUDEPATH += $$PWD/../../Libs

#Communication
LIBS += -L$$DESTDIR -lcommunication

#Interface
INCLUDEPATH += $$PWD/../../Interface

RESOURCES += \
    image.qrc


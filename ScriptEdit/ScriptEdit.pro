QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScriptEditor
TEMPLATE = app
RC_FILE += logo.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS GLOG_NO_ABBREVIATED_SEVERITIES
DEFINES += QSCINTILLA_DLL

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
   BINDIR = $$PWD/../win32-msvc-mes-bin
}
CONFIG(debug, debug|release){
    DESTDIR = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release
}

HEADERS += \
    AddrEditWidget.h \
    FuncManager.h \
    FunctionEditDialog.h \
    mainwindow.h

SOURCES += \
    AddrEditWidget.cpp \
    FuncManager.cpp \
    FunctionEditDialog.cpp \
    main.cpp \
    mainwindow.cpp

FORMS +=

#Qscintilla
INCLUDEPATH += $$PWD/../3rdparty/qscintilla/include
CONFIG(debug, debug|release){
    LIBS += -L$$PWD/../3rdparty/qscintilla/debug/lib/ -lqscintilla2_qt5d
}else{
    LIBS += -L$$PWD/../3rdparty/qscintilla/release/lib/ -lqscintilla2_qt5
}

LIBS +=-L$$DESTDIR -llua

RESOURCES += \
    image.qrc


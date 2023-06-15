QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets axcontainer

TARGET = UiEdit
TEMPLATE = lib
CONFIG += c++17
DEFINES += API_UI

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
    DESTDIR = $$BINDIR/debug/UiSupport/
    libPath = $$BINDIR/debug
}else{
    DESTDIR = $$BINDIR/release/UiSupport/
    libPath = $$BINDIR/release
}

#GLOG
#LIBS += -L$$PWD/../../3rdparty/glog/lib/ -lglog
#INCLUDEPATH += $$PWD/../../3rdparty/glog/include
#App
LIBS += -L$$libPath -lApp
#Tools
LIBS += -L$$libPath -lTools
#Interface
INCLUDEPATH +=  $$PWD/../../Interface

INCLUDEPATH +=$$PWD/../../MesHost/Tools/

#QMAKE_LFLAGS_RELEASE += /MAP
#QMAKE_CFLAGS_RELEASE += /Zi
#QMAKE_LFLAGS_RELEASE += /debug /opt:ref

HEADERS += \
    CustomComponentsDlg.h \
    Gallery/Gallery.h \
    Gallery/GalleryDlg.h \
    Gallery/interface_gallery.h \
    Itemsfactory.h \
    MainStatusBar.h \
    MainWindow.h \
    UIInputPolicy/IUIInputPolicy.h \
    UIInputPolicy/RectCreate_UIInput.h \
    UIInputPolicy/RectEdit_UIInput.h \
    UIInputPolicy/Select_UIInput.h \
    UScenePropertyEditDlg.h \
    UTabWidget.h \
    UToolbar.h \
    UWidgetScene.h \
    UiEdit.h \
    Undo/UAddUndo.h \
    Undo/UDelUndo.h \
    Undo/UMoveUndo.h \
    Undo/URectUndo.h \
    VariableTable/VariableTable.h \
    VariableTable/VariableTableDlg.h \
    VariableTable/interface_variable_table.h

SOURCES += \
    CustomComponentsDlg.cpp \
    Gallery/Gallery.cpp \
    Gallery/GalleryDlg.cpp \
    Itemsfactory.cpp \
    MainStatusBar.cpp \
    MainWindow.cpp \
    UIInputPolicy/RectCreate_UIInput.cpp \
    UIInputPolicy/RectEdit_UIInput.cpp \
    UIInputPolicy/Select_UIInput.cpp \
    UScenePropertyEditDlg.cpp \
    UTabWidget.cpp \
    UToolbar.cpp \
    UWidgetScene.cpp \
    UiEdit.cpp \
    Undo/UAddUndo.cpp \
    Undo/UDelUndo.cpp \
    Undo/UMoveUndo.cpp \
    Undo/URectUndo.cpp \
    VariableTable/VariableTable.cpp \
    VariableTable/VariableTableDlg.cpp

RESOURCES += \
    image.qrc

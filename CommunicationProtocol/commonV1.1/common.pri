
INCLUDEPATH +=$$PWD/

HEADERS += \
    $$PWD/protocol.h \
    $$PWD/protocol_global.h \
    $$PWD/protocol_interface.h \
    $$PWD/protocol_type.h \
    $$PWD/service.h \
    $$PWD/service_def.h \
    $$PWD/typedefheader.h

SOURCES += \
    $$PWD/protocol_extend.cpp \
    $$PWD/protocol_interface.cpp \
    $$PWD/service.cpp \

#GLOG
LIBS += -L$$PWD/../../3rdparty/glog/lib/ -lglog
INCLUDEPATH += $$PWD/../../3rdparty/glog/include


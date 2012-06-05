TEMPLATE = app
TARGET = sync-demo
HEADERS = chatdialog.h \
          digesttreescene.h \
          settingdialog.h \
          treelayout.h

SOURCES = main.cpp \
          chatdialog.cpp \
          digesttreescene.cpp \
          settingdialog.cpp \
          treelayout.cpp 

FORMS = chatdialog.ui \
        settingdialog.ui

QMAKE_CXXFLAGS *= -g 
QMAKE_CFLAGS *= -g 

QMAKE_LIBDIR *= /opt/local/lib /usr/local/lib /usr/lib ../build
INCLUDEPATH *= /opt/local/include /usr/local/include ../include
LIBS *= -lccn -lssl -lcrypto -lpthread -lprotobuf -lsync
CONFIG += console 

PROTOS = chatbuf.proto
include (sync-demo.pri)

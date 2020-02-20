QT += qml quick gui

CONFIG += c++11

HEADERS += \
    QQuickInventorScene.h \
    QQuickInventorView.h
SOURCES += main.cpp \
    QQuickInventorScene.cpp \
    QQuickInventorView.cpp

RESOURCES += QtInventor.qrc

INCLUDEPATH += $$(COINDIR)/include /usr/local/include
LIBS += -L$$(COINDIR)/lib

unix:!macx {
    LIBS += /usr/local/lib/libCoin.so
}

macx: {
    LIBS += /usr/local/lib/libCoin.dylib
}

OTHER_FILES += \
    main.qml

DISTFILES +=

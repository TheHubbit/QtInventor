QT += qml quick gui

CONFIG += c++11

HEADERS += \
    QInventorContext.h \
    QQuickInventorView.h
SOURCES += main.cpp \
    QInventorContext.cpp \
    QQuickInventorView.cpp

RESOURCES += QtInventor.qrc

INCLUDEPATH += $$(COINDIR)/include
LIBS += -L$$(COINDIR)/lib

OTHER_FILES += \
    main.qml

DISTFILES += \
    ExampleScene.iv
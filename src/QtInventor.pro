QT += qml quick gui

CONFIG += c++11

HEADERS += \
    QInventorContext.h \
    QQuickInventor.h
SOURCES += main.cpp \
    QInventorContext.cpp \
    QQuickInventor.cpp

RESOURCES += QtInventor.qrc

INCLUDEPATH += $$(COINDIR)/include
LIBS += -L$$(COINDIR)/lib

OTHER_FILES += \
    main.qml

DISTFILES += \
    ExampleScene.iv

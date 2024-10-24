QT       += core gui   network sql widgets xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat


CONFIG += c++11

TRANSLATIONS += $$PWD/resource/Ch.ts

TRANSLATIONS += \
    zh.ts

TARGET = RobotXQ
TEMPLATE = app


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CRC.cpp \
    Core/BaseThread.cpp \
    DragItems/items/mbaseobject.cpp \
    DragItems/items/mellipseobject.cpp \
    DragItems/items/mlineobject.cpp \
    DragItems/items/mrectobject.cpp \
    DragItems/items/mtextobject.cpp \
    DragItems/scenes/mbasescene.cpp \
    DragItems/views/mbaseview.cpp \
    SCStatusTcp.cpp \
    carmessage.cpp \
    dwwdwqdwqdqwd.cpp \
    main.cpp \
    mycarconnnect.cpp \
    sctcptoolwidget.cpp

HEADERS += \
    CRC.h \
    Core/BaseThread.h \
    DragItems/items/mbaseobject.h \
    DragItems/items/mellipseobject.h \
    DragItems/items/mlineobject.h \
    DragItems/items/mrectobject.h \
    DragItems/items/mtextobject.h \
    DragItems/scenes/mbasescene.h \
    DragItems/views/mbaseview.h \
    SCHeadData.h \
    SCStatusTcp.h \
    carmessage.h \
    dwwdwqdwqdqwd.h \
    mycarconnnect.h \
    sctcptoolwidget.h

FORMS += \
    sctcptoolwidget.ui

    INCLUDEPATH += $$PWD/DragItems/views \
                   $$PWD/DragItems/items \
                   $$PWD/DragItems/scenes


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resource/images/Ellipse.png \
    resource/images/Ellipse_Clicked.png \
    resource/images/Line.png \
    resource/images/Line_Clicked.png \
    resource/images/New.png \
    resource/images/New_Clicked.png \
    resource/images/Open.png \
    resource/images/Open_Clicked.png \
    resource/images/Print.png \
    resource/images/Print_Clicked.png \
    resource/images/Rect.png \
    resource/images/Rect_Clicked.png \
    resource/images/Save.png \
    resource/images/Save_Clicked.png \
    resource/images/Text.png \
    resource/images/Text_Clicked.png \
    resource/images/close.png \
    resource/qss/style.qss

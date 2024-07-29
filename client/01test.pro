QT       += core gui
#指定了项目以来的QT模块 core代表核心模块 gui代表图形化界面，如果需要其他模块，可以在后边继续添加[有手册可查]
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets#如果QT的版本大于4，则会添加widgets模块

CONFIG += c++17#指定了编译器默认使用c++17标准进行编译

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    form.cpp \
    main.cpp \
    talk.cpp \
    widget.cpp

HEADERS += \
    form.h \
    protocol.h \
    talk.h \
    widget.h

FORMS += \
    form.ui \
    talk.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myimg.qrc

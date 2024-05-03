QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lcdemu.cpp \
    main.cpp \
    mainwindow.cpp \
    ssd1306.c \
    ssd1306_font8.c \
    ssd1306_fonts.c \
    swspi.c \
    swspi_port_amd64.c

HEADERS += \
    gpiodef.h \
    lcdemu.h \
    mainwindow.h \
    ssd1306.h \
    ssd1306_conf.h \
    ssd1306_fonts.h \
    swspi.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

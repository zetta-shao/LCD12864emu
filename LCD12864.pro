QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    INA3221.c \
    IP2365.c \
    lcd_font8.c \
    lcd_fontdraw.c \
    lcd_fonts.c \
    lcd_grapdraw.c \
    lcdemu.cpp \
    main.cpp \
    mainwindow.cpp \
    ssd1306.c \
    sw35xx_s.c \
    swi2c.c \
    swi2c_port_amd64.c \
    swspi.c \
    swspi_port_amd64.c

HEADERS += \
    INA3221.h \
    IP2365.h \
    gpiodef.h \
    lcd_font_def.h \
    lcd_fontdraw.h \
    lcd_fonts.h \
    lcdemu.h \
    mainwindow.h \
    ssd1306.h \
    ssd1306_conf.h \
    sw3518_reg.h \
    sw35xx_s.h \
    swi2c.h \
    swspi.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

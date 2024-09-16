#include "mainwindow.h"

#include <QApplication>
#include "gpiodef.h"
#include "swspi.h"
#include "ssd1306.h"
#include "lcdemu.h"

swspi_t sspi1;
SSD1306_t ssd13061;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    int32_t wz_width=SSD1306_WIDTH*SCALE_W_;
    int32_t wz_height=SSD1306_HEIGHT*SCALE_H_;
    LCDemu lcd(w.centralWidget());
    lcd.setlcd(&ssd13061);
    w.resize(wz_width, wz_height+24);
    lcd.setGeometry(0, 0, wz_width, wz_height);
    swspi_HWinit(&sspi1, w.centralWidget());
    w.show();
    lcd.setFocusPolicy(Qt::StrongFocus);
    lcd.start();
    return a.exec();
}

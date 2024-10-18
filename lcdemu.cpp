#include "lcdemu.h"

int16_t vout[8] = { 8302, 6541, 4999, 8998, 12153, 19254 };
int16_t iout[8] = {   -15, -1490, -1951,  -752,  -1335, -1716 };
int16_t vin = 19757;

uint8_t vbat_IP2365(uint16_t val) {
#ifdef ICO_BATTERY_VOLTAGE
    if(val > 7850) return (ICO_BATTERY_VOLTAGE + 4);   //100%
    if(val > 7350) return (ICO_BATTERY_VOLTAGE + 3);   //75%
    if(val > 6850) return (ICO_BATTERY_VOLTAGE + 2);   //50%
    if(val > 6350) return (ICO_BATTERY_VOLTAGE + 1);   //25%
    return (ICO_BATTERY_VOLTAGE + 0);
#else
    (void)val; return 0;
#endif
}

uint8_t vout_IP3518(uint16_t val) {
#ifdef ICO_VOUTVOL
    if(val > 19850) return (ICO_VOUTVOL + 15);  //20v
    if(val > 18850) return (ICO_VOUTVOL + 14);  //19v
    if(val > 17850) return (ICO_VOUTVOL + 13);  //18v
    if(val > 16850) return (ICO_VOUTVOL + 12);  //17v
    if(val > 15850) return (ICO_VOUTVOL + 11);  //16v
    if(val > 14850) return (ICO_VOUTVOL + 10);  //15v
    if(val > 13850) return (ICO_VOUTVOL + 9);  //14v
    if(val > 12850) return (ICO_VOUTVOL + 8);  //13v
    if(val > 11850) return (ICO_VOUTVOL + 7);  //12v
    if(val > 10850) return (ICO_VOUTVOL + 6);  //11v
    if(val > 9850) return (ICO_VOUTVOL + 5);   //10v
    if(val > 8850) return (ICO_VOUTVOL + 4);   //9v
    if(val > 7850) return (ICO_VOUTVOL + 3);   //8v
    if(val > 6850) return (ICO_VOUTVOL + 2);   //7v
    if(val > 5850) return (ICO_VOUTVOL + 1);   //6v
    return (ICO_VOUTVOL + 0);
#else
    (void)val; return 0;
#endif
}

uint8_t out_icon(const char *label) {
#ifdef ICO_BATUM
    /*const uint16_t lbl[10] {
        *(uint16_t*)"b1",
        *(uint16_t*)"b2",
        *(uint16_t*)"a1",
        *(uint16_t*)"a2",
        *(uint16_t*)"a3",
        *(uint16_t*)"a4",
        *(uint16_t*)"c1",
        *(uint16_t*)"c2",
        *(uint16_t*)"c3",
        *(uint16_t*)"c4",
    };
    uint8_t idx;
    uint16_t val=*(uint16_t*)label;
    for(idx=0; idx<10; idx++) {
        if(val==lbl[idx]) return (idx + ICO_BATUM);
    }*/
    uint16_t wlbl = *(uint16_t*)label;
    switch(wlbl) {
    case 0x3162: return (ICO_BATUM + 0); break; //b1
    case 0x3262: return (ICO_BATUM + 1); break; //b2
    case 0x3161: return (ICO_VOUTNUM + 0); break; //a1
    case 0x3261: return (ICO_VOUTNUM + 1); break; //a2
    case 0x3361: return (ICO_VOUTNUM + 2); break; //a3
    case 0x3461: return (ICO_VOUTNUM + 3); break; //a4
    case 0x3163: return (ICO_VOUTNUM + 4); break; //c1
    case 0x3263: return (ICO_VOUTNUM + 5); break; //c2
    case 0x3363: return (ICO_VOUTNUM + 6); break; //c3
    case 0x3463: return (ICO_VOUTNUM + 7); break; //c4
    case 0x7770: return ICO_PWRIN; break; //pw power-in
    case 0x6162: return ICO__BAT; break; //ba battery
    case 0x7375: return ICO__USB; break; //us USB
    default: return 0;
    }
    return 0;
#else
    (void)label; return 0;
#endif
}

#define g_font Font_6x8

LCDemu::LCDemu(QWidget *parent) : QWidget(parent) {
    if(parent) this->setParent(parent);
    m_pQimage = new QImage(SSD1306_WIDTH, SSD1306_HEIGHT, QImage::Format_Grayscale8);
    m_pixmap.convertFromImage(*m_pQimage);
    m_pSWi2c[0] = &m_tSWi2c[0];
    m_pSWi2c[1] = &m_tSWi2c[1];
    m_pSWi2c[2] = &m_tSWi2c[2];
    m_pSWi2c[3] = &m_tSWi2c[3];
    m_pSWspi1 = &m_tSWspi1;
    m_pSSD1306 = &m_tSSD1306;
    m_pINA3221 = &m_tINA3221;
    m_pIP2365[0] = &m_tIP2365[0];
    m_pIP2365[1] = &m_tIP2365[1];
    m_pSW3518[0] = &m_tSW3518[0];
    m_pSW3518[1] = &m_tSW3518[1];
    m_pSW3518[2] = &m_tSW3518[2];
    m_pSW3518[3] = &m_tSW3518[3];
    m_pLCD = &m_tSSD1306.d;
    m_nWidx = 1;
    swspi_HWinit(m_pSWspi1, parent);
    SSD1306_init(m_pSSD1306, m_pSWspi1, &Font_6x8);
    m_qTimer.setSingleShot(false);
    m_qTimer.setInterval(500);
    connect(&m_qTimer, SIGNAL(timeout()), this, SLOT(TimeEvent()) );
}

LCDemu::~LCDemu(void) {
    delete m_pQimage; m_pQimage=nullptr;
    //lcddev=nullptr;
}

void LCDemu::setlcd(SSD1306_t *p) {
    if(!p) return;
    //this->lcddev = p;
    //SSD1306_init(p, &p->d, &Font_6x8);
}

void LCDemu::update(void) {
    int32_t x, y, ift;
    uint8_t *px, *py, pix;
    QColor cH(255,255,255,255), cL(0, 0, 0, 0);
    //if(!m_pSSD1306->d) return;
    px = m_pSSD1306->d.pFrameBuf;
    for(x=0; x<m_pSSD1306->d.frameWidth; x++, px++) {
        py = px;
        for(y=0; y<m_pSSD1306->d.frameHeight; ) {
            pix = *py;
            for(ift=1; ift<255 && y<m_pSSD1306->d.frameHeight; ift<<=1, y++) {
                m_pQimage->setPixelColor(x, y, (pix & ift) ? cH : cL);
            }
            py += m_pSSD1306->d.frameWidth;
        }
    }
    m_pixmap.convertFromImage(m_pQimage->scaled(this->width(), this->height()));
    this->repaint();
}

void LCDemu::paintEvent(QPaintEvent *event) {
    (void)event;
    //if(!m_pSSD1306->d) return;
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->m_pixmap);
    QWidget::paintEvent(event);
}

void LCDemu::keyPressEvent(QKeyEvent *event) {
    int32_t key = event->key();
    if(key == Qt::Key_1) {
        m_nWidx += 1; //task_key1();
    } else if(key == Qt::Key_2) {
        //m_nWidx = 2; //task_key2();
    } else if(key == Qt::Key_3) {
        //m_nWidx = 3; //task_key3();
    } else if(key == Qt::Key_4) {
        //m_nWidx = 4; //task_key4();
    } else if(key == Qt::Key_Escape) {
        exit(0);
    }
    if(m_nWidx > 4) m_nWidx = 1;
    if(m_nWidx < 1) m_nWidx = 4;
    //QKeyEvent::KeyPressEvent(event);
}

void LCDemu::TimeEvent(void) {
    switch(m_nWidx) {
    case 1: task_key1(); break;
    case 2: task_key2(); break;
    case 3: task_key3(); break;
    case 4: task_key4(); break;
    }
    vout[0] = ina3221_getVol_mV(m_pINA3221, 0);
    iout[0] = ina3221_getCur_mA(m_pINA3221, 0);
    vout[1] = ina3221_getVol_mV(m_pINA3221, 1);
    iout[1] = ina3221_getCur_mA(m_pINA3221, 1);
    SW35xx_readStatus(m_pSW3518[0], 1);
    vout[2] = m_pSW3518[0]->vout_mV;
    if(m_pSW3518[0]->iout_usba_mA != 0)
        iout[2] = m_pSW3518[0]->iout_usba_mA;
    else
        iout[2] = m_pSW3518[0]->iout_usbc_mA;
    m_qTimer.start();
}

void LCDemu::start(void) {
    //int orgV;
    int i;
    swi2c_HWinit(m_pSWi2c[0], (void*)"/dev/i2c-0");
    swi2c_HWinit(m_pSWi2c[1], (void*)"/dev/i2c-1");
    swi2c_HWinit(m_pSWi2c[2], (void*)"/dev/i2c-2");
    swi2c_HWinit(m_pSWi2c[3], (void*)"/dev/i2c-3");
    IP2365_init(m_pIP2365[0], m_pSWi2c[0]);
    IP2365_init(m_pIP2365[1], m_pSWi2c[1]);
    ina3221_init(m_pINA3221, m_pSWi2c[0]);
    sw35xx_init(m_pSW3518[0], m_pSWi2c[0]);
    sw35xx_init(m_pSW3518[1], m_pSWi2c[1]);
    for(i=0; i<2; i++) {
        if(m_pIP2365[i]->pD == nullptr) continue;
        ip2365_enable(m_pIP2365[i], 0);
        ip2365_set_Isetbat_mA(m_pIP2365[i], 1500);
        ip2365_set_Isetvin_mA(m_pIP2365[i], 750);
    }
    if(m_tINA3221.pDev) {
        ina3221_setAveragingMode(m_pINA3221, INA3221_REG_CONF_AVG_256);
        ina3221_setBusConversionTime(m_pINA3221, INA3221_REG_CONF_CT_1100US);
        ina3221_setShuntConversionTime(m_pINA3221, INA3221_REG_CONF_CT_1100US);
        //orgV = (int)ina3221_getVol_mV(&m_tINA3221, 0);
    }
    m_pLCD->fill_color(m_pLCD, 0);
    update();
    m_qTimer.start();
    for(i=0; i<2; i++) {
        if(m_pIP2365[i]->pD == nullptr) continue;
        ip2365_enable(m_pIP2365[i], 1);
    }
}

void LCDemu::task_key1(void) {
    char str[64];
    char icons[][3] = { "b1", "b2", "c1", "c2", "c3", "c4" };
    //char lstr1[8], lstr2[8], lstr3[8];
    lcddev_t *d = m_pLCD;
    __strfmt str_format = &str_3digitL;
    //ssd1306_Fill(m_pSSD1306, Black);
    //fontdraw_fill(d, 0);
    m_pLCD->fill_color(m_pLCD, 0);
    if(1) {
        fontdraw_setpos(d, 0, 0);
        fontdraw_string(d, (char*)"ou vol cur  wat     ");        
        fontdraw_setpos(d, 0, d->frameHeight - 8);
        sprintf(str, "%c:", out_icon("pw"));
        fontdraw_string(d, str);
        fontdraw_setpos(d, 12, d->frameHeight - 8);
        str_format(vin, str);
        fontdraw_string(d, str);
    }
    if(1) {
        str[0] = vbat_IP2365(vout[0]);
        str[1] = vbat_IP2365(vout[1]);
        str[2] = vout_IP3518(vout[2]);
        str[3] = vout_IP3518(vout[3]);
        str[4] = vout_IP3518(vout[4]);
        str[5] = vout_IP3518(vout[5]);
        str[6] = 0;
        fontdraw_setpos(d, d->frameWidth - 36, d->frameHeight - 8);
        fontdraw_string(d, str);
    }
    if(1) {
        uint32_t p, idx; int8_t np;
        int yoft;
        for(idx=0, yoft=8; idx<6; idx++, yoft+=8) {
            p = (vout[idx] * iout[idx]) / 1000;

            fontdraw_setpos(d, 0, yoft);
            sprintf(str, "%c:", out_icon(icons[idx]));
            fontdraw_stringC(d, str, 1);

            np = str_format(vout[idx], str);
            fontdraw_setpos(d, 12, yoft);
            fontdraw_stringC(d, str, np);

            np = str_format(iout[idx], str);
            fontdraw_setpos(d, 42, yoft);
            fontdraw_stringC(d, str, np);

            np = str_format(p, str);
            fontdraw_setpos(d, 72, yoft);
            fontdraw_stringC(d, str, np);
        }

    }
    if(0){
        int x;
        fontdraw_setpos(d, 0, 8);
        for(x=0; x<16; x++) {
            fontdraw_drawpixelBW(d, x, x, 1);
            fontdraw_drawpixelBW(d, 15-x, x, 1);
            fontdraw_drawpixelBW(d, 16+x, x, 1);
            fontdraw_drawpixelBW(d, 31-x, x, 1);
            fontdraw_drawpixelBW(d, x, x+16, 1);
            fontdraw_drawpixelBW(d, 15-x, x+16, 1);
            fontdraw_drawpixelBW(d, 16+x, x+16, 1);
            fontdraw_drawpixelBW(d, 31-x, x+16, 1);
        }
    }

    update();
}
void LCDemu::task_key2(void) {
    lcddev_t *d = &m_pSSD1306->d;
    int i, y, res, neg;
    char str[8];
    m_pLCD->fill_color(m_pLCD, 0);
    fontdraw_setpos(d, 0, 0);
    fontdraw_string(d, (char*)"IP2365 1/2");
    //fontdraw_setpos(d, 64, 0);
    //fontdraw_stringC(d, (char*)"2keyevent", 1);
    for(i=0, y=8; i<2; i++, y+=32) {
        if(m_pIP2365[i]->pD == nullptr) {
            fontdraw_setpos(d, 0, y);
            fontdraw_stringC(d, (char*)"not found", 0);
            continue;
        }

        fontdraw_setpos(d, 0, y);
        if(ip2365_get_ChargeEnable(m_pIP2365[i]))
            fontdraw_stringC(d, (char*)"On", 1);
        else
            fontdraw_stringC(d, (char*)"of", 0);

        res = ip2365_get_chg_stat_cicv(m_pIP2365[i]);
        fontdraw_setpos(d, 0, 8+y);
        if(res==1) fontdraw_stringC(d, (char*)"CC", 1);
        else if(res ==3) fontdraw_stringC(d, (char*)"CV", 1);
        else fontdraw_stringC(d, (char*)"tr", 0);

        res = ip2365_get_Isetbat_mA(m_pIP2365[i]);
        fontdraw_setpos(d, 18, y);
        if(res == 65535)
            fontdraw_stringC(d, (char*)"-def-", 0);
        else {
            str_5digit_m(res, str, 'A');
            fontdraw_stringC(d, str, 1);
        }

        res = iout[0]; //ina3221_getCur_mA(m_pINA3221, 0);
        fontdraw_setpos(d, 18, 8+y);
        neg = str_5digit_m(res, str, 'A');
        fontdraw_stringC(d, str, neg);

        res = ip2365_get_Isetvin_mA(m_pIP2365[i]);
        fontdraw_setpos(d, 54, y);
        if(res == 65535)
            fontdraw_stringC(d, (char*)"-def-", 0);
        else {
            str_5digit_m(res, str, 'A');
            fontdraw_stringC(d, str, 1);
        }

        res = vout[0]; //ina3221_getVol_mV(m_pINA3221, 0);
        fontdraw_setpos(d, 54, 8+y);
        neg = str_5digit_m(res, str, 'V');
        fontdraw_stringC(d, str, neg);

        res = ip2365_get_chg_stat_full(m_pIP2365[i]);
        fontdraw_setpos(d, 90, y);
        if(res) fontdraw_stringC(d, (char*)"ful", 0);
        fontdraw_stringC(d, (char*)"chr", 1);
    }
    update();

}
void LCDemu::task_key3(void) {
    lcddev_t *d = &m_pSSD1306->d;
    m_pLCD->fill_color(m_pLCD, 0);
    fontdraw_setpos(d, 0, 0);
    fontdraw_string(d, (char*)"keyevent");
    fontdraw_setpos(d, 64, 0);
    fontdraw_stringC(d, (char*)"keyevent", 1);
    update();

}
void LCDemu::task_key4(void) {
    char str[64];
    lcddev_t *d = &m_pSSD1306->d;
    m_pLCD->fill_color(m_pLCD, 0);
    fontdraw_setpos(d, 0, 0);
    fontdraw_string(d, (char*)"ABCDEFGHI");
    fontdraw_setpos(d, 0, 8);
    fontdraw_string(d, (char*)"JKLMNOPQR");
    fontdraw_setpos(d, 0, 16);
    fontdraw_string(d, (char*)"STUVWXYZ!");
    fontdraw_setpos(d, 64, 0);
    fontdraw_stringC(d, (char*)"abcdefghi", 0);
    fontdraw_setpos(d, 64, 8);
    fontdraw_stringC(d, (char*)"jklmnopqr", 0);
    fontdraw_setpos(d, 64, 16);
    fontdraw_stringC(d, (char*)"stuvwxyz!", 0);

    {
        int i, v=1;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;

        fontdraw_setpos(d, 0, 24);
        fontdraw_string(d, str);
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        fontdraw_setpos(d, 64, 24);
        fontdraw_string(d, str);

        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        fontdraw_setpos(d, 0, 32);
        fontdraw_string(d, str);

        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        fontdraw_setpos(d, 64, 32);
        fontdraw_string(d, str);

        v = 118;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        fontdraw_setpos(d, 0, 40);
        fontdraw_string(d, str);

        v = 128;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        fontdraw_setpos(d, 64, 40);
        fontdraw_string(d, str);

        v = 138;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        fontdraw_setpos(d, 0, 48);
        fontdraw_string(d, str);

        v = 148;
        for(i=0; i<2; i++, v++) str[i] = v;
        str[i] = 0;
        fontdraw_setpos(d, 64, 48);
        fontdraw_string(d, str);

    }
    update();
}

#include "lcdemu.h"

uint16_t vout[8] = { 8302, 6541, 4999, 8998, 12153, 19254 };
uint16_t iout[8] = {   15, 1490, 1951,  752,  1335, 1716 };
uint16_t vin = 19757;

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

void str_3digit(uint16_t val, char *outstr) {
#if defined (ICO_NUMBER_DOT) && defined(ICO_DOT_NUMBER)
    if(val < 1000) {
        sprintf(outstr, " %02d", val%100);
        outstr[0] = ((val/100)%10) + ICO_DOT_NUMBER;
    } else if(val < 10000) {
        sprintf(outstr, " %02d", (val%1000)/10);
        outstr[0] = ((val/1000)%10) + ICO_NUMBER_DOT;
    } else if(val < 65535) {
        sprintf(outstr, "%01d %01d", (val/10000)%10, (val%1000)/100);
        outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    }
    outstr[3] = 0;
#else
    (void)val; (void)outstr;
#endif
}

void str_3digitL(uint16_t val, char *outstr) { //3+1
    if(val < 1000) {
        sprintf(outstr, ".%03d", val);
    } else if(val < 10000) {
        sprintf(outstr, "%01d.%02d", (val/1000)%10, (val%1000)/10);
    } else if(val < 65535) {
        sprintf(outstr, "%02d.%01d", (val/1000)%100, (val%1000)/100);
    }
    outstr[4] = 0;
}

void str_4digit(uint16_t val, char *outstr) {
#ifdef ICO_NUMBER_DOT
    if(val < 10000) {
        sprintf(outstr, " %03d", val%1000);
        outstr[0] = ((val/1000)%10) + ICO_NUMBER_DOT;
    } else if(val < 65535) {
        sprintf(outstr, "%01d %02d", (val/10000)%10, (val%1000)/10);
        outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    }
    outstr[4] = 0;
#else
    (void)val; (void)outstr;
#endif
}

void str_4digitL(uint16_t val, char *outstr) { //4+1
    if(val < 10000) {
        sprintf(outstr, "%01d.%03d", (val/1000)%10, val%1000);
    } else if(val < 65535) {
        sprintf(outstr, "%02d.%02d", (val/1000)%100, (val%1000)/10);
    }
    outstr[5] = 0;
}


void str_5digit(uint16_t val, char *outstr) {
#ifdef ICO_NUMBER_DOT
    if(val < 10000)
        sprintf(outstr, "  %03d", val%1000);
    else
        sprintf(outstr, "%01d %03d", (val/10000)%10, val%1000);

    outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    outstr[5] = 0;
#else
    (void)val; (void)outstr;
#endif
}

typedef void (*__strfmt)(uint16_t, char*);
#define g_font Font_6x8

LCDemu::LCDemu(QWidget *parent) : QWidget(parent) {
    if(parent) this->setParent(parent);
    m_pQimage = new QImage(SSD1306_WIDTH, SSD1306_HEIGHT, QImage::Format_Grayscale8);
    m_pixmap.convertFromImage(*m_pQimage);
    lcddev = nullptr;
}

LCDemu::~LCDemu(void) {
    delete m_pQimage; m_pQimage=nullptr;
    lcddev=nullptr;
}

void LCDemu::setlcd(SSD1306_t *p) {
    if(!p) return;
    this->lcddev = p;
}

void LCDemu::update(void) {
    int32_t x, y, ift;
    uint8_t *px, *py, pix;
    QColor cH(255,255,255,255), cL(0, 0, 0, 0);
    if(!lcddev) return;
    px = lcddev->SSD1306_Buffer;
    for(x=0; x<SSD1306_WIDTH; x++, px++) {
        py = px;
        for(y=0; y<SSD1306_HEIGHT; ) {
            pix = *py;
            for(ift=1; ift<255 && y<SSD1306_HEIGHT; ift<<=1, y++) {
                m_pQimage->setPixelColor(x, y, (pix & ift) ? cH : cL);
            }
            py += SSD1306_WIDTH;
        }
    }
    m_pixmap.convertFromImage(m_pQimage->scaled(this->width(), this->height()));
    this->repaint();
}

void LCDemu::paintEvent(QPaintEvent *event) {
    (void)event;
    if(!lcddev) return;
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->m_pixmap);
    QWidget::paintEvent(event);
}

void LCDemu::keyPressEvent(QKeyEvent *event) {
    int32_t key = event->key();
    if(key == Qt::Key_1) {
        task_key1();
    } else if(key == Qt::Key_2) {
        task_key2();
    } else if(key == Qt::Key_3) {
        task_key3();
    } else if(key == Qt::Key_4) {
        task_key4();
    }
    //QKeyEvent::KeyPressEvent(event);
}

void LCDemu::start(void) {
    task_key1();
}

void LCDemu::task_key1(void) {
    char str[64], lstr1[8], lstr2[8], lstr3[8];
    FontDef _f = g_font;
    __strfmt str_format = &str_3digitL;
    ssd1306_Fill(lcddev, Black);
    ssd1306_SetCursor(lcddev, 0, 0); //6x6=36
    ssd1306_WriteString(lcddev, (char*)"out vol  cur wat     ", _f, White);

    {
        //str_2dot3(vin, lstr1);
        str_format(vin, lstr1);
        sprintf(str, "%ci:%s            ", out_icon("pw"), lstr1);
        ssd1306_SetCursor(lcddev, 0, SSD1306_HEIGHT-8); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);
    }
    {
        str[0] = vbat_IP2365(vout[0]);
        str[1] = vbat_IP2365(vout[1]);
        str[2] = vout_IP3518(vout[2]);
        str[3] = vout_IP3518(vout[3]);
        str[4] = vout_IP3518(vout[4]);
        str[5] = vout_IP3518(vout[5]);
        str[6] = 0;
        ssd1306_SetCursor(lcddev, SSD1306_WIDTH - 36, SSD1306_HEIGHT-8); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);
    }

    {
        uint32_t p;
        p = (vout[0] * iout[0]) / 1000;
        str_format(vout[0], lstr1);
        str_format(iout[0], lstr2);
        str_format(p, lstr3);
        //sprintf(str, "%c1:%s %s %s    ", 'b', lstr1, lstr2, lstr3);
        sprintf(str, "%c:%s %s %s    ", out_icon("b1"), lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 8); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);

        p = (vout[1] * iout[1]) / 1000;
        str_format(vout[1], lstr1);
        str_format(iout[1], lstr2);
        str_format(p, lstr3);
        //sprintf(str, "%c2:%s %s %s    ",  'b', lstr1, lstr2, lstr3);
        sprintf(str, "%c:%s %s %s    ",  out_icon("b2"), lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 16); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);

        p = (vout[2] * iout[2]) / 1000;
        str_format(vout[2], lstr1);
        str_format(iout[2], lstr2);
        str_format(p, lstr3);
        //sprintf(str, "%c1:%s %s %s    ", 'A', lstr1, lstr2, lstr3);
        sprintf(str, "%c:%s %s %s    ",  out_icon("a1"), lstr1, lstr2, lstr3);
        if(vout[2] > 10000) str[3] = (vout[2]/10000) + '0';
        ssd1306_SetCursor(lcddev, 0, 24); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);

        p = (vout[3] * iout[3]) / 1000;
        str_format(vout[3], lstr1);
        str_format(iout[3], lstr2);
        str_format(p, lstr3);
        //sprintf(str, "%c2:%s %s %s    ", 'A', lstr1, lstr2, lstr3);
        sprintf(str, "%c:%s %s %s    ",  out_icon("a2"), lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 32); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);

        p = (vout[4] * iout[4]) / 1000;
        str_format(vout[4], lstr1);
        str_format(iout[4], lstr2);
        str_format(p, lstr3);
        //sprintf(str, "%c3:%s %s %s    ", 'c', lstr1, lstr2, lstr3);
        sprintf(str, "%c:%s %s %s    ",  out_icon("c3"), lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 40); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);

        p = (vout[5] * iout[5]) / 1000;
        str_format(vout[5], lstr1);
        str_format(iout[5], lstr2);
        str_format(p, lstr3);
        //sprintf(str, "%c4:%s %s %s    ", 'c', lstr1, lstr2, lstr3);
        sprintf(str, "%c:%s %s %s    ",  out_icon("c4"), lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 48); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, White);

    }
    update();
}
void LCDemu::task_key2(void) {
    FontDef _f = g_font;
    ssd1306_Fill(lcddev, Black);
    ssd1306_SetCursor(lcddev, 0, 0);
    ssd1306_WriteString(lcddev, (char*)"2keyevent", _f, White);
    ssd1306_SetCursor(lcddev, 64, 0);
    ssd1306_WriteString(lcddev, (char*)"2keyevent", _f, Black);
    update();

}
void LCDemu::task_key3(void) {
    FontDef _f = g_font;
    ssd1306_Fill(lcddev, Black);
    ssd1306_SetCursor(lcddev, 0, 0);
    ssd1306_WriteString(lcddev, (char*)"key_event", _f, White);
    ssd1306_SetCursor(lcddev, 64, 0);
    ssd1306_WriteString(lcddev, (char*)"key_event", _f, Black);
    update();

}
void LCDemu::task_key4(void) {
    char str[64];
    FontDef _f = g_font;
    ssd1306_Fill(lcddev, Black);
    ssd1306_SetCursor(lcddev, 0, 0);
    ssd1306_WriteString(lcddev, (char*)"ABCDEFGHI", _f, White);
    ssd1306_SetCursor(lcddev, 0, 8);
    ssd1306_WriteString(lcddev, (char*)"JKLMNOPQR", _f, White);
    ssd1306_SetCursor(lcddev, 0, 16);
    ssd1306_WriteString(lcddev, (char*)"STUVWXYZ!", _f, White);
    ssd1306_SetCursor(lcddev, 64, 0);
    ssd1306_WriteString(lcddev, (char*)"abcdefghi", _f, Black);
    ssd1306_SetCursor(lcddev, 64, 8);
    ssd1306_WriteString(lcddev, (char*)"jklmnopqr", _f, Black);
    ssd1306_SetCursor(lcddev, 64, 16);
    ssd1306_WriteString(lcddev, (char*)"stuvwxyz!", _f, Black);

    {
        int i, v=1;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 0, 24);
        ssd1306_WriteString(lcddev, str, _f, White);
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 64, 24);
        ssd1306_WriteString(lcddev, str, _f, White);

        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 0, 32);
        ssd1306_WriteString(lcddev, str, _f, White);

        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 64, 32);
        ssd1306_WriteString(lcddev, str, _f, White);

        v = 118;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 0, 40);
        ssd1306_WriteString(lcddev, str, _f, White);

        v = 128;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 64, 40);
        ssd1306_WriteString(lcddev, str, _f, White);

        v = 138;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 0, 48);
        ssd1306_WriteString(lcddev, str, _f, White);

        v = 148;
        for(i=0; i<2; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 64, 48);
        ssd1306_WriteString(lcddev, str, _f, White);

    }
    update();
}

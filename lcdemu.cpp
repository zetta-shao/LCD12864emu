#include "lcdemu.h"

uint16_t vout[8] = { 8302, 6541, 4999, 8998, 12153, 19254 };
uint16_t iout[8] = {   15, 1490, 1951,  752,  1335, 1716 };
uint16_t vin = 19757;

uint8_t vbat_IP2365(uint16_t val) {
    uint8_t res = 138-127;      //0%
    if(val > 6350) res++;   //25%
    if(val > 6850) res++;   //50%
    if(val > 7350) res++;   //75%
    if(val > 7850) res++;   //100%
    return res;
}

uint8_t vout_IP3518(uint16_t val) {
    uint8_t res = 143-127;  //5v
    if(val > 5850) res++;   //6v
    if(val > 6850) res++;   //7v
    if(val > 7850) res++;   //8v
    if(val > 8850) res++;   //9v
    if(val > 9850) res++;   //10v
    if(val > 10850) res++;  //11v
    if(val > 11850) res++;  //12v
    if(val > 12850) res++;  //13v
    if(val > 13850) res++;  //14v
    if(val > 14850) res++;  //15v
    if(val > 15850) res++;  //16v
    if(val > 16850) res++;  //17v
    if(val > 17850) res++;  //18v
    if(val > 18850) res++;  //19v
    if(val > 19850) res++;  //20v
    return res;
}

uint8_t out_icon(const char *label) {
    const uint16_t lbl[10] {
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
        if(val==lbl[idx]) return idx+127;
    }
    return 0;
}

void str_1dot2(uint16_t val, char *outstr) {
    sprintf(outstr, " %02d", (val % 1000)/10);
    outstr[0] = ((val / 1000) % 10) + 1;
    outstr[3] = 0;
}

void str_2dot2(uint16_t val, char *outstr) {
    sprintf(outstr, "  %02d", (val % 1000)/10);
    outstr[1] = ((val / 1000) % 10) + 1;
    if(val > 9999) outstr[0] = ((val / 10000) % 10) + 48;
    outstr[4] = 0;
}

void str_1dot3(uint16_t val, char *outstr) {
    sprintf(outstr, " %03d", val % 1000);
    outstr[0] = ((val / 1000) % 10) + 1;
    outstr[4] = 0;
}

void str_2dot3(uint16_t val, char *outstr) {
    sprintf(outstr, "  %03d", val % 1000);
    outstr[1] = ((val / 1000) % 10) + 1;
    if(val > 9999) outstr[0] = ((val / 10000) % 10) + 48;
    outstr[5] = 0;
}

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
    FontDef _f = Font_5x8;
    ssd1306_Fill(lcddev, Black);
    ssd1306_SetCursor(lcddev, 0, 0); //6x6=36
    ssd1306_WriteString(lcddev, (char*)"out  vol  cur  wat     ", _f, Black);
    update();

    {
        str_2dot3(vin, lstr1);
        sprintf(str, "%ci:%s            ", 137, lstr1);
        ssd1306_SetCursor(lcddev, 0, SSD1306_HEIGHT-8); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, Black);
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
        ssd1306_WriteString(lcddev, str, _f, Black);

        update();
    }

    {
        uint32_t p;
        p = (vout[0] * iout[0]) / 1000;
        str_2dot3(vout[0], lstr1);
        str_1dot3(iout[0], lstr2);
        str_2dot2(p, lstr3);
        sprintf(str, "%c1:%s %s %s    ", 'b', lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 8); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, Black);

        p = (vout[1] * iout[1]) / 1000;
        str_2dot3(vout[1], lstr1);
        str_1dot3(iout[1], lstr2);
        str_2dot2(p, lstr3);
        sprintf(str, "%c2:%s %s %s    ",  'b', lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 16); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, Black);

        p = (vout[2] * iout[2]) / 1000;
        str_2dot3(vout[2], lstr1);
        str_1dot3(iout[2], lstr2);
        str_2dot2(p, lstr3);
        sprintf(str, "%c1:%s %s %s    ", 'A', lstr1, lstr2, lstr3);
        if(vout[2] > 10000) str[3] = (vout[2]/10000) + '0';
        ssd1306_SetCursor(lcddev, 0, 24); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, Black);

        p = (vout[3] * iout[3]) / 1000;
        str_2dot3(vout[3], lstr1);
        str_1dot3(iout[3], lstr2);
        str_2dot2(p, lstr3);
        sprintf(str, "%c2:%s %s %s    ", 'A', lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 32); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, Black);

        p = (vout[4] * iout[4]) / 1000;
        str_2dot3(vout[4], lstr1);
        str_1dot3(iout[4], lstr2);
        str_2dot2(p, lstr3);
        sprintf(str, "%c3:%s %s %s    ", 'c', lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 40); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, Black);

        p = (vout[5] * iout[5]) / 1000;
        str_2dot3(vout[5], lstr1);
        str_1dot3(iout[5], lstr2);
        str_2dot2(p, lstr3);
        sprintf(str, "%c4:%s %s %s    ", 'c', lstr1, lstr2, lstr3);
        ssd1306_SetCursor(lcddev, 0, 48); //6x6=36
        ssd1306_WriteString(lcddev, str, _f, Black);

        update();
    }
}
void LCDemu::task_key2(void) {
    FontDef _f = Font_5x8;
    ssd1306_Fill(lcddev, Black);
    ssd1306_SetCursor(lcddev, 0, 0);
    ssd1306_WriteString(lcddev, (char*)"2keyevent", _f, White);
    ssd1306_SetCursor(lcddev, 64, 0);
    ssd1306_WriteString(lcddev, (char*)"2keyevent", _f, Black);
    update();

}
void LCDemu::task_key3(void) {
    FontDef _f = Font_5x8;
    ssd1306_Fill(lcddev, Black);
    ssd1306_SetCursor(lcddev, 0, 0);
    ssd1306_WriteString(lcddev, (char*)"key_event", _f, White);
    ssd1306_SetCursor(lcddev, 64, 0);
    ssd1306_WriteString(lcddev, (char*)"key_event", _f, Black);
    update();

}
void LCDemu::task_key4(void) {
    char str[64];
    FontDef _f = Font_5x8;
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
        ssd1306_WriteString(lcddev, str, _f, Black);

        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 0, 32);
        ssd1306_WriteString(lcddev, str, _f, White);

        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 64, 32);
        ssd1306_WriteString(lcddev, str, _f, Black);

        v = 118;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 0, 40);
        ssd1306_WriteString(lcddev, str, _f, White);

        v = 128;
        for(i=0; i<10; i++, v++) str[i] = v;
        str[i] = 0;
        ssd1306_SetCursor(lcddev, 64, 40);
        ssd1306_WriteString(lcddev, str, _f, Black);

    }
    update();
}
